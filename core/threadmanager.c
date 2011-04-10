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



#define _CRT_SECURE_NO_WARNINGS

#ifdef Darwin
#	include <CoreServices/CoreServices.h>
#endif

#include "queue_internal.h"

/* Once-only initialisation of the key */
static dispatch_once_t _dispatch_buffer_key_once = 0;
/* Key for the thread-specific buffer */
static pthread_key_t _dispatch_buffer_key;

// run once to allocate the thread keys
static void _buffer_key_alloc(void* unused){
    int r = pthread_key_create(&_dispatch_buffer_key, NULL);
    assert(r==0);
}

void _set_thread_queue(dispatch_queue_t q){
    int r = pthread_setspecific(_dispatch_buffer_key, q);
    assert(r==0);
}

dispatch_queue_t _get_thread_queue(){
	return (dispatch_queue_t)pthread_getspecific(_dispatch_buffer_key);
}

void* _thread(void* q){
	_thread_t me = (_thread_t)q;

	dispatch_once_f(&_dispatch_buffer_key_once, NULL, _buffer_key_alloc);

        while(!me->loop){
			sleep(1);
        }

	_evt_run(me->loop, 0);

	return NULL;
}

void* _timer_thread(void* q){
	_evt_loop_t loop = (_evt_loop_t)q;

	_evt_run(loop,0);

	return NULL;
}

void _spawn_thread(_thread_t t){

	int rc = pthread_create(&t->id, NULL, _thread, t);
#ifdef DEBUG
	if(rc){
#		ifdef _MSC_VER
		char error[100];
		strerror_s(error,99,rc);
		printf("ERROR: Could not spawn needed thread: %s\n",error);
#		else
		printf("ERROR: Could not spawn needed thread: %s\n",strerror(rc));
#		endif
	}
#else
	rc = 0;
#endif
}

void _spawn_timer_thread(void* q){
	pthread_t newThread;
	int rc = pthread_create(&newThread, NULL, _timer_thread, q);
#ifdef DEBUG
	if(rc){
#		ifdef _MSC_VER
		char error[100];
		strerror_s(error,99,rc);
		printf("ERROR: Could not spawn needed thread: %s\n",error);
#		else
		printf("ERROR: Could not spawn needed thread: %s\n",strerror(rc));
#		endif
	}
#else
	rc = 0;
#endif
}

void _check_threads(_threadpool_t p){
	// TODO Make this more sophisticated
	if(_tp_count(p) < MAX_THREADS) {
		//printf("Spawn\n");
		_tp_add(p);
		if(_tp_count(p)<2){
			//printf("Spawn\n");
			_tp_add(p);
		}
	}
}

unsigned int _ideal_thread_count(){

	static unsigned int ct = 0;
	if ( ct == 0) {
#ifdef WIN32
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		ct = sysinfo.dwNumberOfProcessors;
#else
#	ifdef Darwin
		ct = MPProcessorsScheduled();
#	else
		ct = (int)sysconf(_SC_NPROCESSORS_ONLN);
#	endif
#endif
		if (ct < 3) ct = 3;
	}

	return ct;

}
