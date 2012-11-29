#ifndef DISPATCH_WIN_PTHREAD_COND_VARIABLES_
#define DISPATCH_WIN_PTHREAD_COND_VARIABLES_

typedef int pthread_condattr_t;

static int pthread_condattr_destroy(pthread_condattr_t *a)
{
	(void) a;
	return 0;
}

#define pthread_condattr_getclock(A, C) ENOTSUP
#define pthread_condattr_setclock(A, C) ENOTSUP

static int pthread_condattr_init(pthread_condattr_t *a)
{
	*a = 0;
	return 0;
}

static int pthread_condattr_getpshared(pthread_condattr_t *a, int *s)
{
	*s = *a;
	return 0;
}

static int pthread_condattr_setpshared(pthread_condattr_t *a, int s)
{
	*a = s;
	return 0;
}


#if TARGET_OS_WIN_XP

/* Credits for this pthread_cond_t implementation on windows go to
the authors of http://www.cs.wustl.edu/~schmidt/win32-cv-1.html:
Douglas C. Schmidt and Irfan Pyarali
Department of Computer Science
Washington University, St. Louis, Missouri
*/
typedef struct
{
	int waiters_count_;
	// Count of the number of waiters.

	CRITICAL_SECTION waiters_count_lock_;
	// Serialize access to <waiters_count_>.

	int release_count_;
	// Number of threads to release via a <pthread_cond_broadcast> or a
	// <pthread_cond_signal>.

	int wait_generation_count_;
	// Keeps track of the current "generation" so that we don't allow
	// one thread to steal all the "releases" from the broadcast.

	HANDLE event_;
	// A manual-reset event that's used to block and release waiting
	// threads.
} pthread_cond_t;

static int pthread_cond_init (pthread_cond_t *cv, const pthread_condattr_t * attr)
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

static int pthread_cond_wait (pthread_cond_t *cv, pthread_mutex_t *external_mutex)
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

static int pthread_cond_signal (pthread_cond_t *cv)
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

static int pthread_cond_broadcast (pthread_cond_t *cv)
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

static int pthread_cond_destroy(pthread_cond_t *cv)
{
	CloseHandle(cv->event_);

	return 0;
}


#else

/*
 * Posix Threads library for Microsoft Windows
 *
 * Use at own risk, there is no implied warranty to this code.
 * It uses undocumented features of Microsoft Windows that can change
 * at any time in the future.
 *
 * (C) 2010 Lockless Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of Lockless Inc. nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AN
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

typedef CONDITION_VARIABLE pthread_cond_t;

static int pthread_cond_init(pthread_cond_t *c, pthread_condattr_t *a)
{
	(void) a;
	
	InitializeConditionVariable(c);
	return 0;
}

static int pthread_cond_signal(pthread_cond_t *c)
{
	WakeConditionVariable(c);
	return 0;
}

static int pthread_cond_broadcast(pthread_cond_t *c)
{
	WakeAllConditionVariable(c);
	return 0;
}

static int pthread_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)
{
	pthread_testcancel();
	SleepConditionVariableCS(c, m, INFINITE);
	return 0;
}

static int pthread_cond_destroy(pthread_cond_t *c)
{
	(void) c;
	return 0;
}

static int pthread_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m, struct timespec *t)
{
	DWORD tm = (DWORD)_pthread_rel_time_in_ms(t);
	
	pthread_testcancel();
	
	if (!SleepConditionVariableCS(c, m, tm)) return ETIMEDOUT;
	
	// We can have a spurious wakeup after the timeout
	if (!_pthread_rel_time_in_ms(t)) return ETIMEDOUT;
	
	return 0;
}

#endif

#endif /* DISPATCH_WIN_PTHREAD_COND_VARIABLES_ */
