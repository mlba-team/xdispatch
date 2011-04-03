/*
 * Copyright (c) 2010 Apple Inc. All rights reserved.
 * Copyright (c) 2011 MLBA. All rights reserved.
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
	param->arg = arg;
	param->start_routine = start_routine;

    *thread = CreateThread(0, 0, thread_starter, param, 0, 0 );
    // TODO: Proper error handling
    return *thread == 0;
}

int sem_init(sem_t * sem, int flag, unsigned int val){
    *sem = CreateSemaphore(0, 1, 1, 0);
    // TODO: Proper error handling
    return *sem == 0;
}

int sem_timedwait(sem_t * sem, const struct timespec * timeout){

    DWORD duration = timeout->tv_nsec / NSEC_PER_MSEC + timeout->tv_sec * 1000;

    switch(WaitForSingleObject((sem),duration) ){
    case WAIT_TIMEOUT:
        return ETIMEDOUT;
    case WAIT_FAILED:
        return EINVAL;
    default:
        return 0;
    }
}

/* Credits for this pthread_cond_t implementation on windows go to
   the authors of http://www.cs.wustl.edu/~schmidt/win32-cv-1.html:
        Douglas C. Schmidt and Irfan Pyarali
        Department of Computer Science
        Washington University, St. Louis, Missouri
*/

int
pthread_cond_init (pthread_cond_t *cv,
                   const pthread_condattr_t * attr)
{
    cv->waiters_count_ = 0;
    cv->wait_generation_count_ = 0;
    cv->release_count_ = 0;

	// Init the second mutex
	InitializeCriticalSection(&cv->waiters_count_lock_);

    // Create a manual-reset event.
    cv->event_ = CreateEvent (NULL,  // no security
                              TRUE,  // manual-reset
                              FALSE, // non-signaled initially
                              NULL); // unnamed

	return cv->event_ == 0;
}

int
pthread_cond_wait (pthread_cond_t *cv,
                   pthread_mutex_t *external_mutex)
{
	int my_generation = 0;
	int wait_done = 0;
	int last_waiter = 0;

    // Avoid race conditions.
    EnterCriticalSection (&cv->waiters_count_lock_);

    // Increment count of waiters.
    cv->waiters_count_++;

    // Store current generation in our activation record.
    my_generation = cv->wait_generation_count_;

    LeaveCriticalSection (&cv->waiters_count_lock_);
    LeaveCriticalSection (external_mutex);

    for (;;) {
        // Wait until the event is signaled.
        WaitForSingleObject (cv->event_, INFINITE);

        EnterCriticalSection (&cv->waiters_count_lock_);
        // Exit the loop when the <cv->event_> is signaled and
        // there are still waiting threads from this <wait_generation>
        // that haven't been released from this wait yet.
        wait_done = cv->release_count_ > 0
                && cv->wait_generation_count_ != my_generation;
        LeaveCriticalSection (&cv->waiters_count_lock_);

        if (wait_done)
            break;
    }

    EnterCriticalSection (external_mutex);
    EnterCriticalSection (&cv->waiters_count_lock_);
    cv->waiters_count_--;
    cv->release_count_--;
    last_waiter = cv->release_count_ == 0;
    LeaveCriticalSection (&cv->waiters_count_lock_);

    if (last_waiter)
        // We're the last waiter to be notified, so reset the manual event.
        ResetEvent (cv->event_);

	return 0;
}

int
pthread_cond_signal (pthread_cond_t *cv)
{
    EnterCriticalSection (&cv->waiters_count_lock_);
    if (cv->waiters_count_ > cv->release_count_) {
        SetEvent (cv->event_); // Signal the manual-reset event.
        cv->release_count_++;
        cv->wait_generation_count_++;
    }
    LeaveCriticalSection (&cv->waiters_count_lock_);

	return 0;
}

int
pthread_cond_broadcast (pthread_cond_t *cv)
{
    EnterCriticalSection (&cv->waiters_count_lock_);
    if (cv->waiters_count_ > 0) {
        SetEvent (cv->event_);
        // Release all the threads in this generation.
        cv->release_count_ = cv->waiters_count_;

        // Start a new generation.
        cv->wait_generation_count_++;
    }
    LeaveCriticalSection (&cv->waiters_count_lock_);

	return 0;
}

int
pthread_cond_destroy(pthread_cond_t *cv)
{
    CloseHandle(cv->event_);

	return 0;
}

#endif
