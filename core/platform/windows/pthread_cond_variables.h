#ifndef DISPATCH_WIN_PTHREAD_COND_VARIABLES_
#define DISPATCH_WIN_PTHREAD_COND_VARIABLES_

// These conditions are supported using Windows Vista and up only. We need a XP compatible workaround here
/*
typedef CONDITION_VARIABLE pthread_cond_t;
#define pthread_cond_init(c,x) InitializeConditionVariable((c))
#define pthread_cond_destroy(c)
#define pthread_cond_wait(c,m) SleepConditionVariableCS((c),(m), INFINITE)
#define pthread_cond_broadcast(c) WakeAllConditionVariable((c));
#define pthread_cond_signal(c) WakeConditionVariable((c));
*/

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

typedef struct {
	int unused;
} pthread_condattr_t;

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

#endif /* DISPATCH_WIN_PTHREAD_COND_VARIABLES_ */
