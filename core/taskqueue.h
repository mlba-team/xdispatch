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



#ifndef TASK_QUEUE_H_
#define TASK_QUEUE_H_

#include "queue_internal.h"
#include "events.h"

#ifdef _WIN32
#	define _d_suspend Sleep
#else
#	define _d_suspend usleep
#endif

#ifdef DEBUG
# define Q_EXPORT DISPATCH_EXPORT
#else
# define Q_EXPORT DISPATCH_EXPORT
#endif

#define TQ_SAFE_ENTER(Q) while(atomic_swap_get(&(Q->access),1)==1) /*{ _d_suspend(1); } */
#define TQ_SAFE_LEAVE(Q) atomic_swap_get(&(Q->access),0)

typedef void (*task_function)(void *);

typedef struct _taskitem_s {
	void* context;
	task_function func;
	struct _taskitem_s* next;
	unsigned char serial;
    //clock_t prio;
    clock_t timeout;
}* _taskitem_t;

typedef struct _taskqueue_s {
	_taskitem_t first;
	_taskitem_t last;
	ATOMIC_INT access;
	// a task queue can have an event loop, it will only be executed on this loop
	_evt_loop_t loop;
	char* name;
	// The queue has a worker assigned
    ATOMIC_INT active_worker;
    // or belongs to a thread pool
    pthread_workqueue_t pool;
}* _taskqueue_t;

//
//	Returns a new tasksqueue
//
Q_EXPORT struct _taskqueue_s* _tq_create();
//
//	Sets a name for the queue
//	
Q_EXPORT void _tq_name(_taskqueue_t, const char* name);
//
//	Initializes the given queue
//	
Q_EXPORT void _tq_init(struct _taskqueue_s* t);
//
//	Removes all items from the given queue
//	
Q_EXPORT void _tq_clear(struct _taskqueue_s* t);
//
//	Destroys the given queue
//	
Q_EXPORT void _tq_destroy(struct _taskqueue_s* t);
//
//	Removes the given item from the given queue
//	
Q_EXPORT void _tq_remove(struct _taskqueue_s* t,_taskitem_t i);

//
//	Tests if the given queue is empty
//	
#define _tq_empty(t) (char)(t->first==NULL)
//
//	Initializes the given task queue item
//	
static inline void _tq_init_item(_taskitem_t n){memset(n,0,sizeof(struct _taskitem_s));}
//
//	Contructs a new task queue item
//	
static inline _taskitem_t _tq_item(){ 
	_taskitem_t n = (struct _taskitem_s*)malloc(sizeof(struct _taskitem_s)); 
	if(n) 
		memset(n,0,sizeof(struct _taskitem_s)); 
	return n; 
}
//
//	Deletes the given item
//	
#define _tq_delete(i) { free(i); i = NULL; }
//
//	Returns the last item in the queue or NULL if no item is contained
//	
#define _tq_last(t) t->last
//
//	Appends the given item to the end of the given queue
//	
static inline void _tq_append(struct _taskqueue_s* t,_taskitem_t i){
	TQ_SAFE_ENTER(t);
	if(t->last)
		t->last->next = i;
	else
		t->first = i;
	t->last = i;
	TQ_SAFE_LEAVE(t);
}
//
//	Pushes the given item in front of all other items in the queue
//	
static inline void _tq_push(struct _taskqueue_s* t,_taskitem_t i){
	TQ_SAFE_ENTER(t); 
	i->next = t->first; 
	t->first = i;
	if(!t->last)
		t->last = i;
	TQ_SAFE_LEAVE(t);
}
//
//	Returns and removes the first item from the queue
//	
static inline _taskitem_t _tq_pop(struct _taskqueue_s* t){ 
	_taskitem_t r = NULL; 
	TQ_SAFE_ENTER(t); 
	r = t->first; 
	if(r){
		t->first = r->next;
		if(!r->next)
			t->last = NULL;
	}
	TQ_SAFE_LEAVE(t); 
	return r;
}
//
//	Returns the first item of the queue
//	
#define _tq_first(t) t->first
//
//	Returns the number of item currently in the queue
//	
static inline int _tq_count(struct _taskqueue_s* t){ 
	int erg=0; 
	_taskitem_t pos=NULL; 
	TQ_SAFE_ENTER(t); 
	pos=t->first; 
	while(pos) {
		erg++; 
		pos=pos->next;
	} 
	TQ_SAFE_LEAVE(t); 
	return erg; 
}


#endif /* TASK_QUEUE_H_ */
