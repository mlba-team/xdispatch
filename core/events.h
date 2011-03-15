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



#ifndef EVENTS_H_
#define EVENTS_H_

#ifdef WIN32
#	include "../pthreads-w32/pthread.h"
#else
#	include <pthread.h>
#endif

//
//	A signle event item, used internal only
//	
typedef struct _evt_event_s {
	int type;
	void* data;
    struct _evt_event_s* next;
}* _evt_event_t;

#define _EVT_EVENT_INITIALIZER { 0, NULL, NULL }

//
//	An event loop, normally you have one loop per thread
//	
typedef struct _evt_loop_s {
	int id;
	void* data;
    void (*cb)(struct _evt_loop_s*, int, void*);
    _evt_event_t first;
    _evt_event_t last;
	pthread_mutex_t access;
	pthread_cond_t cond;
	unsigned char used;
}* _evt_loop_t;

#define _EVT_LOOP_INITIALIZER { 0, NULL, NULL, NULL, NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, TRUE }

//
//	A function according to this callback will be called
//	to handle an incoming event
//	
typedef void(*_evt_handler)(_evt_loop_t, int, void*);

//
//	The different event types
//	
enum _evt_type {
	EVT_WAKEUP = 0, /* Simply wake the thread, the callback will know what to do */
	EVT_EXIT, /* Immediately exit the event loop */
	EVT_CUSTOM /* Some custom event */
};

//
//	Flags controling the behaviour ov evt_run
//	
enum _evt_flags {
	EVT_NONE = 0, /* No flag (default) */
	EVT_NOWAIT /* Process all pending events and return immediately */
};

//
//	Creates a new event loop passing all incoming events 
//	to the given callback h
//	
_evt_loop_t _evt_create(_evt_handler h);

//
//  Initializes an existing eventloop (as if it was new)
//
void _evt_init(_evt_loop_t t, _evt_handler h);

//
//	Destroys the given event loop, make sure
//	that evt-run returned before. This will 
//	erase all pending events as well
//	
void _evt_destroy(_evt_loop_t l);

//
//	Sends an event of type type with the given data 
//	to the given event loop
//	
int _evt_signal(_evt_loop_t l, int type, void* data);

//
//	Executes the given event loop.
//	Will return in two casees only:
//		- EVT_NOWAIT was passed as flag
//		- An event of type exit was posted
//	
void _evt_run(_evt_loop_t l, int flags);

#endif /* EVENTS_H_ */
