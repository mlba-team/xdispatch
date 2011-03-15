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



#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include "events.h"

#define TP_SAFE_ENTER(Q) while(atomic_swap_get(&(Q->access),1)==1)
#define TP_SAFE_LEAVE(Q) atomic_swap_get(&(Q->access),0)

//
//	A single thread
//
typedef struct _thread_s {
	_evt_loop_t loop;
	struct _thread_s* next;
	pthread_t id;
}* _thread_t;

//
//	Initializes an empty struct _threadpool_s
//	
#define TP_EMPTY_POOL { NULL, _EVT_LOOP_INITIALIZER, 0, 0 }

typedef struct _threadpool_s {
	_thread_t first;
	struct _evt_loop_s loop;
	ATOMIC_INT access;
	unsigned int count;
}* _threadpool_t;

//
//	Signals the given thread
//	
#define _tp_signal__thread(t) _evt_signal(t->loop, EVT_WAKEUP, 0)

//
//	creates and returns the threadpool
//	
_threadpool_t _tp_create();
//
//	Entirely tdestroys the given pool
//	
void _tp_destroy(_threadpool_t t);
//
//	Removes the given thread
//	
void _tp_remove(_threadpool_t t, _thread_t i);
//
//	Sends an event to all threads
//	
void _tp_signal(_threadpool_t p);
//
//	Adds a thread to the threadpool
//	
_thread_t _tp_add(_threadpool_t t);
//
//	Returns the number of threads currently in pool
//	
#define _tp_count(t) t->count
//
//	Returns the next thread in the pool. NULL if it doesn't exist.
//	
static inline _thread_t _tp_next(_threadpool_t p, _thread_t current){ TP_SAFE_ENTER(p); current = current->next; TP_SAFE_LEAVE(p); return current; }
//
//	Returns the first thread in the pool
//	
#define _tp_first(p) p->first


#endif /* THREADPOOL_H_ */
