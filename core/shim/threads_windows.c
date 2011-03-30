/*
 * Copyright (c) 2010 Apple Inc. All rights reserved.
 *
 * @APPLE_APACHE_LICENSE_HEADER_START@
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
 * @APPLE_APACHE_LICENSE_HEADER_END@
 */

#include "../internal.h"

#ifdef _WIN32

pthread_key_t dispatch_queue_key;
pthread_key_t dispatch_sema4_key;
pthread_key_t dispatch_cache_key;
pthread_key_t dispatch_bcounter_key;

struct thread_param {
	void *(*start_routine)(void*);
	void* arg;
};

static DWORD WINAPI thread_starter( LPVOID lpParam ) {

	struct thread_param* param = (struct thread_param*)(lpParam);
	assert(lpParam);
	param->start_routine(param->arg);
	free(param);

	return 0;
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    void *(*start_routine)(void*), void *arg){
        /*
HANDLE WINAPI CreateThread(
  __in_opt   LPSECURITY_ATTRIBUTES lpThreadAttributes,
  __in       SIZE_T dwStackSize,
  __in       LPTHREAD_START_ROUTINE lpStartAddress,
  __in_opt   LPVOID lpParameter,
  __in       DWORD dwCreationFlags,
  __out_opt  LPDWORD lpThreadId
);
*/
	struct thread_param* param = (struct thread_param*) malloc(sizeof(struct thread_param));
	assert(param);

    *thread = CreateThread(0, 0, thread_starter, param, 0, 0 );
    // TODO: Proper error handling
    return *thread == 0;
}

int sem_init(sem_t * sem, int flag, unsigned int val){
    *sem = CreateSemaphore(0, 1, 1, 0);
    // TODO: Proper error handling
    return *sem == 0;
}

#endif
