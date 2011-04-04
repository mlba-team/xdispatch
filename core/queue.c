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


#include "queue_internal.h"

static dispatch_queue_t _dispatch_main_q = NULL;
static _taskqueue_t _dispatch_event_q = NULL;
static const char* _dispatch_global_queues[] = { "com.apple.root.low-priority", "com.apple.root.default-priority", "com.apple.root.high-priority" };
dispatch_queue_t _dispatch_global_q[] = { NULL, NULL, NULL };

#ifdef _WIN32
static struct _threadpool_s _dispatch_pool;

// DllMain - see http://msdn.microsoft.com/en-us/library/ms682596%28v=vs.85%29.aspx
BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason )
    {
        case DLL_PROCESS_ATTACH:
         // Initialize once for each new process.
         // Return FALSE to fail DLL load.
            _dispatch_pool = *(_tp_create());
            break;

        case DLL_PROCESS_DETACH:
         // Perform any necessary cleanup.
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

#else
static struct _threadpool_s _dispatch_pool = TP_EMPTY_POOL;
#endif

dispatch_queue_t dispatch_get_main_queue(){
	// test for existing main queue
	if(_dispatch_main_q==NULL) {
		// if not create a new one
		_dispatch_main_q = dispatch_queue_create("com.apple.main-thread",NULL);
		cast_queue(_dispatch_main_q)->loop = _evt_create(_loop_worker);
	}
	return _dispatch_main_q;
}

//
// Signals the workers that a new item is available
//
inline void _signal(dispatch_queue_t queue){
		_taskqueue_t q = cast_queue(queue);

		// wake a sleeping thread to handle our task
		if(q->loop)
			_evt_signal(q->loop, EVT_WAKEUP, queue);
		else {
			// make sure there are enough threads to handle this
			_check_threads(&_dispatch_pool);
			// TODO does it make sense to wake all threads every time or should we only wake one
			_tp_signal(&_dispatch_pool);
		}
}

//
// Dispatches an existing item without notification to the workers
//
inline void _dispatch_async_fast_exists_f(dispatch_queue_t queue, _taskitem_t i){
		_taskqueue_t q = cast_queue(queue);
		dispatch_retain(queue);

        assert(queue->type == DISPATCH_QUEUE || queue->type == DISPATCH_SERIAL_QUEUE);
		_tq_append(q,i);
}

//
// Dispatches a function without notification to the workers
//
inline void _dispatch_async_fast_f(dispatch_queue_t queue, void *context, dispatch_function_t work){
		_taskitem_t i = _tq_item();

		assert(queue);
		assert(work);

		i->serial = FALSE;
		i->context = context;
		i->func = work;
		i->prio = clock();
		_dispatch_async_fast_exists_f(queue, i);
}

//
// Dispatches an existing item
//
inline void _dispatch_async_exists_f(dispatch_queue_t queue, _taskitem_t i){

		assert(queue);
		assert(i);
		
		_dispatch_async_fast_exists_f(queue, i);
		_signal(queue);
}

void
	dispatch_async_f(dispatch_queue_t queue, void *context, dispatch_function_t work){
		_taskqueue_t q = cast_queue(queue);
		
		assert(queue);
		assert(work);
		
		_dispatch_async_fast_f(queue, context, work);

		if(queue->type == DISPATCH_SERIAL_QUEUE) {
			OBJ_SAFE_ENTER(queue);
			if( atomic_swap_get(&(q->worker),1)==0)
				dispatch_async_f(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,0), queue, _serial_worker);
			OBJ_SAFE_LEAVE(queue);
		} else {
			_signal(queue);
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
					_dispatch_async_fast_f(queue, dt, _apply_helper);
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
		if( label == NULL || strstr(label,"com.apple.") == NULL )
			neu->type = DISPATCH_SERIAL_QUEUE;
		else
			neu->type = DISPATCH_QUEUE;
		_tq_init(cast_queue(neu));

		q = cast_queue(neu);
		_tq_init(q);
                if(label)
                    _tq_name(q,label);

		if(neu->type == DISPATCH_SERIAL_QUEUE)
			q->worker = 0;
		else
			q->worker = 1;

		return neu;
}

const char *
	dispatch_queue_get_label(dispatch_queue_t queue){
		if(queue->type != DISPATCH_QUEUE && queue->type != DISPATCH_SERIAL_QUEUE)
			return "";

		return cast_queue(queue)->name==0 ? "" : cast_queue(queue)->name;
}

void
	dispatch_set_target_queue(dispatch_object_t object, dispatch_queue_t queue){
		assert(object);

		object->target = queue;
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
		item->prio = (clock_t)(when/NSEC_PER_MSEC);

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
