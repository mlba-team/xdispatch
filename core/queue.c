/*
' Copyright (c) 2008-2009 Apple Inc. All rights reserved.
* Copyright (c) 2011 MLBA. All rights reserved.
*
* @MLBA_OPEN_LICENSE_HEADER_START@
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
*     http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* @MLBA_OPEN_LICENSE_HEADER_END@
*/


#include "internal.h"

DISPATCH_EXPORT dispatch_queue_t _dispatch_main_q = NULL;

dispatch_queue_t dispatch_get_main_queue(){
    return _dispatch_main_q;
}

static _taskqueue_t _dispatch_event_q = NULL;

const char* _dispatch_global_queues[] = {
    "com.apple.root.low-priority",
    "com.apple.root.default-priority",
    "com.apple.root.high-priority"
};
dispatch_queue_t _dispatch_global_q[] = { NULL, NULL, NULL };

// the priorities assigned to the work queues
static inline int
_dispatch_rootq2wq_pri(long idx)
{
    switch (idx) {
    case 0:
        return WORKQ_LOW_PRIOQUEUE;
    case 1:
    default:
        return WORKQ_DEFAULT_PRIOQUEUE;
    case 2:
        return WORKQ_HIGH_PRIOQUEUE;
    }
}

void _dispatch_root_queues_init(){
    int i = 0;
    int r;
    pthread_workqueue_attr_t wq_attr;

    // create global queues
    r = pthread_workqueue_attr_init_np(&wq_attr);
    dispatch_assume_zero(r);

    for(i = 0; i < 3; i++) {
        _dispatch_global_q[i] = dispatch_queue_create(_dispatch_global_queues[i],NULL);
        assert(cast_queue(_dispatch_global_q[i]));
        r = pthread_workqueue_attr_setqueuepriority_np(&wq_attr, _dispatch_rootq2wq_pri(i));
        dispatch_assume_zero(r);
        r = pthread_workqueue_attr_setovercommit_np(&wq_attr, i & 1);
        dispatch_assume_zero(r);
        r = pthread_workqueue_create_np(&(cast_queue(_dispatch_global_q[i])->pool), &wq_attr);
        dispatch_assume_zero(r);
        dispatch_assume( cast_queue(_dispatch_global_q[i])->pool );
        _dispatch_global_q[i]->type = DISPATCH_QUEUE;
    }

    pthread_workqueue_attr_destroy_np(&wq_attr);

    // and the main queue
    _dispatch_main_q = dispatch_queue_create("com.apple.main-thread", NULL);
    dispatch_assume( cast_queue(_dispatch_main_q) );
    _dispatch_main_q->type = DISPATCH_MAIN_QUEUE;
    cast_queue(_dispatch_main_q)->loop = _evt_create(_main_worker);
    dispatch_assume( cast_queue(_dispatch_main_q)->loop );
}

#ifdef __BLOCKS__
dispatch_block_t
_dispatch_Block_copy(dispatch_block_t db)
{
    dispatch_block_t rval;

    while (!(rval = Block_copy(db))) {
        sleep(1);
    }

    return rval;
}
#define _dispatch_Block_copy(x) ((typeof(x))_dispatch_Block_copy(x))

void
_dispatch_call_block_and_release(void *block)
{
    void (^b)(void) = block;
    b();
    Block_release(b);
}

void
_dispatch_call_block_and_release2(void *block, void *ctxt)
{
    void (^b)(void*) = block;
    b(ctxt);
    Block_release(b);
}

#endif /* __BLOCKS__ */

//
// Dispatches an existing item without notification to the workers
//
void _dispatch_async_fast_exists_f(dispatch_queue_t queue, _taskitem_t i){
    _taskqueue_t q = cast_queue(queue);
    dispatch_retain(queue);

    assert(queue->type == DISPATCH_QUEUE || queue->type == DISPATCH_SERIAL_QUEUE || queue->type == DISPATCH_MAIN_QUEUE);
    _tq_append(q,i);
}

//
// Dispatches a function without notification to the workers
//
void _dispatch_async_fast_f(dispatch_queue_t queue, void *context, dispatch_function_t work){
    _taskitem_t i = _tq_item();

    assert(queue);
    assert(work);

    i->serial = FALSE;
    i->context = context;
    i->func = work;
    //i->prio = clock();
    _dispatch_async_fast_exists_f(queue, i);
}

//
// Dispatches an existing item
//
/* inline void _dispatch_async_exists_f(dispatch_queue_t queue, _taskitem_t i){

    assert(queue);
    assert(i);

    _dispatch_async_fast_exists_f(queue, i);
    _signal(queue);
} */

#ifdef __BLOCKS__
void
dispatch_async(dispatch_queue_t dq, void (^work)(void))
{
    dispatch_async_f(dq, _dispatch_Block_copy(work), _dispatch_call_block_and_release);
}
#endif

void dispatch_async_f(dispatch_queue_t queue, void *context, dispatch_function_t work){
    dispatch_queue_t target;
    _taskqueue_t q = cast_queue(queue);

    assert(queue);
    assert(work);

    _dispatch_async_fast_f(queue, context, work);

    switch(queue->type) {
    case DISPATCH_SERIAL_QUEUE:
    {
        OBJ_SAFE_ENTER(queue);
        if( atomic_swap_get(&(q->active_worker),1)==0 ) {
            target = queue->target ? queue->target : dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,0);
            dispatch_async_f(target, queue, _serial_worker);
        }
        OBJ_SAFE_LEAVE(queue);
    }
        break;
    case DISPATCH_MAIN_QUEUE:
    {
        _notify_main(queue);
    }
        break;
    case DISPATCH_QUEUE:
    {
        _notify_global(queue);
    }
        break;
    }
}

struct sync_dt_s {
	void* context;
	char done;
	dispatch_function_t work;
};

// Used for implementing dispatch_sync_f
void _sync_helper(void* dt){
	struct sync_dt_s* s = (struct sync_dt_s*)dt;

	assert(dt);

	s->work(s->context);
	s->done = 1;
}

#ifdef __BLOCKS__
void
dispatch_sync(dispatch_queue_t dq, void (^work)(void))
{
    struct Block_basic *bb = (void *)work;
    dispatch_sync_f(dq, work, (dispatch_function_t)bb->Block_invoke);
}
#endif

void
dispatch_sync_f(dispatch_queue_t queue,	void *context, dispatch_function_t work){

    assert(queue);
    assert(work);

    // go sleeping, the executing thread will wake us
    if(queue != _dispatch_main_q){
        struct sync_dt_s s = { context, 0, work };
        dispatch_async_f(queue, &s, _sync_helper);
        while(s.done==0)
#ifdef WIN32
            Sleep(1);
#else
            usleep(1000);
#endif
    } else
        work(context);
}

struct apply_dt_s {
	void (*work)(void*, size_t);
	void* context;
	size_t ct;
};

void _apply_helper(void* dt){
	struct apply_dt_s* s = (struct apply_dt_s*)dt;

	assert(dt);

	s->work(s->context,s->ct);
	free(dt);
}

#ifdef __BLOCKS__
void
dispatch_apply(size_t iterations, dispatch_queue_t dq, void (^work)(size_t))
{
    struct Block_basic *bb = (void *)work;

    dispatch_apply_f(iterations, dq, bb, (void *)bb->Block_invoke);
}
#endif

void
dispatch_apply_f(size_t iterations, dispatch_queue_t queue, void *context, void (*work)(void *, size_t)){
    size_t i = 0;

    assert(queue);
    assert(work);

    for(i = 0; i < iterations; i++){
        struct apply_dt_s* dt = (struct apply_dt_s*)malloc(sizeof(struct apply_dt_s));
        if(dt) {
            dt->work = work; dt->ct = i; dt->context = context;
            if(i==iterations-1)
                dispatch_sync_f(queue, dt, _apply_helper);
            else
                dispatch_async_f(queue, dt, _apply_helper);
        }
    }
}

dispatch_queue_t dispatch_get_current_queue(void){
	dispatch_queue_t curr = _get_thread_queue();
	if(!curr)
		return dispatch_get_main_queue();

	return _get_thread_queue();
}

dispatch_queue_t
dispatch_get_global_queue(long priority, unsigned long flags){
    int selection = 1;

    switch(priority) {
    case DISPATCH_QUEUE_PRIORITY_LOW:
        selection = 0;
        break;
    case DISPATCH_QUEUE_PRIORITY_HIGH:
        selection = 2;
        break;
    default:
        selection = 1;
    }

    // test if the queue exists
    if(_dispatch_global_q[selection]==NULL) {
        // if no create a new one
        _dispatch_global_q[selection] = dispatch_queue_create(_dispatch_global_queues[selection],NULL);
    }
    return _dispatch_global_q[selection];
}

dispatch_queue_t
dispatch_queue_create(const char *label, dispatch_queue_attr_t attr){
    dispatch_queue_t neu;
    _taskqueue_t q;

    neu = (dispatch_queue_t)_get_empty_object();
    neu->type = DISPATCH_SERIAL_QUEUE;
    _tq_init(cast_queue(neu));

    q = cast_queue(neu);
    _tq_init(q);
    if(label)
        _tq_name(q,label);

    if(neu->type == DISPATCH_SERIAL_QUEUE)
        q->active_worker = 0;
    else
        q->active_worker = 1;

    return neu;
}

const char *
dispatch_queue_get_label(dispatch_queue_t queue){
    if(queue->type != DISPATCH_QUEUE && queue->type != DISPATCH_SERIAL_QUEUE)
        return "";

    return cast_queue(queue)->name==0 ? "" : cast_queue(queue)->name;
}

void dispatch_main(void){

	_evt_run(cast_queue(dispatch_get_main_queue())->loop, 0);

}

struct timer_dt_s {
	dispatch_function_t work;
	void* context;
	dispatch_queue_t queue;
};

void _timer(void* context){
	struct timer_dt_s* dt = (struct timer_dt_s*)context;

	assert(context);

	dispatch_async_f(dt->queue, dt->context, dt->work);
	free(dt);

}

#ifdef __BLOCKS__
void
dispatch_after(dispatch_time_t when, dispatch_queue_t queue, dispatch_block_t work)
{
    // test before the copy of the block
    if (when == DISPATCH_TIME_FOREVER) {
#ifdef DEBUG
        printf("dispatch_after() called with 'when' == infinity");
#endif
        return;
    }
    dispatch_after_f(when, queue, _dispatch_Block_copy(work), _dispatch_call_block_and_release);
}
#endif

void dispatch_after_f(dispatch_time_t when, dispatch_queue_t queue, void *context, dispatch_function_t work){
    static unsigned int guard = 0;
    struct timer_dt_s* dt;
    _taskitem_t item = _tq_item();

    assert(queue);
    assert(work);

    if(when == DISPATCH_TIME_NOW){
        dispatch_async_f(queue, context, work);
        return;
    }

    dt = (struct timer_dt_s*)malloc(sizeof(struct timer_dt_s));
    if(!dt)
        return;

    dt->context = context;
    dt->queue = queue;
    dt->work = work;

    item->context = dt;
    item->func = _timer;
    item->timeout = (clock_t)(when/NSEC_PER_MSEC);

    // make sure the event loop actually runs
    while(atomic_swap_get(&(guard),1)==1);
    if(!_dispatch_event_q){
        _dispatch_event_q = _tq_create();
        _dispatch_event_q->loop = _evt_create(_timer_worker);
        _spawn_timer_thread(_dispatch_event_q->loop);
    }
    atomic_swap_get(&(guard),0);

    _tq_append(_dispatch_event_q, item);
    _evt_signal(_dispatch_event_q->loop, EVT_WAKEUP, _dispatch_event_q);
}
