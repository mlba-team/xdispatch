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



#include "queue_internal.h"

/**
Selects the queue with the highest priority item
*/
dispatch_queue_t _select_queue(){
	// TODO we need a better bonus calculation time
	clock_t bonus_times[3] = { 
		BONUS_LOW,  
		BONUS_DEFAULT,
		BONUS_HIGH
	};

	dispatch_queue_t selection = NULL, current = NULL;
	_taskitem_t task = NULL;
	clock_t selection_prio = -1;
	clock_t prio = -1;
	int ct = 0;

	for(ct = 2; ct >= 0; ct--){
		current = _dispatch_global_q[ct];
		if(current==NULL)
			continue;

		task = _tq_first(cast_queue(current));
		if(task == NULL)
			continue;

		prio = task->prio - bonus_times[ct];
		//printf("%u ",(unsigned int)prio);
		if(selection==NULL || prio < selection_prio) {
			selection = current;
			selection_prio = prio;
		}
	}

	//printf("selected %u (Queues %i, %i, %i)\n", (unsigned int)selection_prio, (int)global_q[0], (int)global_q[1], (int)global_q[2]);
	//dispatch_debug(global_q[0], "");
	//dispatch_debug(global_q[1], "");
	//dispatch_debug(global_q[2], "");

	return selection;
}

void _loop_worker(_evt_loop_t loop, int revent, void* data){
	dispatch_queue_t q = (dispatch_queue_t)(data);
	dispatch_queue_t queue = NULL;
	_taskitem_t task = NULL;

	if(q) {
		queue = q;
	} else {
		queue = _select_queue();
	}
	_set_thread_queue(queue);

	while(queue && !_tq_empty(cast_queue(queue))){
		if(!q) {
			queue = _select_queue();
			if(!queue)
				return;
		}
		task = _tq_pop(cast_queue(queue));
		if(task) {
			task->func(task->context);
			_tq_delete(task);
			dispatch_release(queue);
		}
	}
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
			if(i->prio <= now) {
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
	char should_release = FALSE;
	_taskqueue_t queue = NULL;
	_taskitem_t task = NULL;
	dispatch_queue_t o = (dispatch_queue_t)(context);

	assert(context);
	assert(o->type == DISPATCH_SERIAL_QUEUE);

	queue = cast_queue(o);

	_set_thread_queue(o);

	if(!o->suspended) {
		task = _tq_pop(queue);
		if(task) {
			task->func(task->context);
			should_release = TRUE;
		}
	}

	OBJ_SAFE_ENTER(o);
	// Ensure only one worker is running at a time
	if(_tq_empty(queue)) {
		atomic_swap_get(&queue->worker,0);
		_tq_delete(task);
	} else {
		if(task) {
			_tq_init_item(task);
			task->context = context;
			task->func = _serial_worker;
			task->prio = clock();
			task->serial = FALSE;
			_dispatch_async_fast_exists_f(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,0), task);
		} else
			_dispatch_async_fast_f(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), context, _serial_worker);
	}
	OBJ_SAFE_LEAVE(o);

	if(should_release) dispatch_release(o);
}
