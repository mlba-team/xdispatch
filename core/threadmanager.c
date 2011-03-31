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

/* Key for the thread-specific buffer */
static pthread_key_t _dispatch_buffer_key;

// run once to allocate the thread keys
void _thread_man_init(void){
	pthread_key_create(&_dispatch_buffer_key, NULL);
}

void _thread_man_cleanup(void){
    // nothing to do so far
}

void _set_thread_queue(dispatch_queue_t q){
	pthread_setspecific(_dispatch_buffer_key, q);
}

dispatch_queue_t _get_thread_queue(){
	return (dispatch_queue_t)pthread_getspecific(_dispatch_buffer_key);
}

void* _timer_thread(void* q){
	_evt_loop_t loop = (_evt_loop_t)q;

	_evt_run(loop,0);

	return NULL;
}

void _spawn_timer_thread(void* q){
	pthread_t newThread;
	int rc = pthread_create(&newThread, NULL, _timer_thread, q);
#ifdef DEBUG
	if(rc){
#		ifdef _WIN32
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

