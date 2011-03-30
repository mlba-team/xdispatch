/*
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

void _loop_worker(void* ct) {
    dispatch_queue_t q = (dispatch_queue_t)ct;
    _taskitem_t task;

    _set_thread_queue(q);


    task = _tq_pop(cast_queue(q));
    if(task) {
        task->func(task->context);
        _tq_delete(task);
        dispatch_release(q);
    }

}

void _main_worker(_evt_loop_t loop, int revent, void *data){
    dispatch_queue_t q = (dispatch_queue_t)data;
    assert(data);

    while(!_tq_empty( cast_queue(q) ))
        _loop_worker(data);
}

void _timer_worker(_evt_loop_t loop, int revent, void* data){
    _taskitem_t i = NULL;
	_taskqueue_t q = (_taskqueue_t)data;
	size_t ct = 0, count = 0;

	assert(data);

	while(!_tq_empty(q)){
		time_t now = (time_t)( dispatch_time(0,0) / NSEC_PER_MSEC );

		count = _tq_count(q);
		for(ct = 0; ct < count; ct++){
			i = _tq_pop(q);
            if(i->timeout <= now) {
				i->func(i->context);
				_tq_delete(i);
			} else {
				_tq_append(q,i);
			}
		}

#ifdef WIN32
		Sleep(1);
#else
		usleep(100);
#endif
	}
}

void _serial_worker(void* context){
    /*char should_release = FALSE;
 _taskqueue_t queue = NULL;
    _taskitem_t task = NULL;*/
    dispatch_queue_t target = NULL;
	dispatch_queue_t o = (dispatch_queue_t)(context);

	assert(context);
	assert(o->type == DISPATCH_SERIAL_QUEUE);
    //queue = cast_queue(o);
    while(!_tq_empty(cast_queue(o)))
        _loop_worker(o);
    /*
    if(o->suspended <= 0) {
  task = _tq_pop(queue);
  if(task) {
   task->func(task->context);
   should_release = TRUE;
  }
 }
*/
	OBJ_SAFE_ENTER(o);
	// Ensure only one worker is running at a time
    if(_tq_empty(cast_queue(o))) {
        atomic_swap_get(&cast_queue(o)->active_worker,0);
        //_tq_delete(task);
    } else {
        /*if(task) {
   _tq_init_item(task);
   task->context = context;
   task->func = _serial_worker;
   task->prio = clock();
   task->serial = FALSE;
   _dispatch_async_fast_exists_f(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,0), task);
        } else */
        target = o->target ? o->target : dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
        dispatch_async_f(target, context, _serial_worker);
	}
	OBJ_SAFE_LEAVE(o);

    //	if(should_release) dispatch_release(o);
}

void _notify_main(dispatch_queue_t q){
    assert(q);
    assert(cast_queue(q)->loop);
    _evt_signal(cast_queue(q)->loop, EVT_WAKEUP, q);
}

void _notify_global(dispatch_queue_t q){
    assert(q);
    assert(cast_queue(q)->pool);

    pthread_workqueue_additem_np(cast_queue(q)->pool, _loop_worker, (void*)q, NULL, NULL);
}
