#ifndef DISPATCH_WIN_PTHREAD_RWLOCK_
#define DISPATCH_WIN_PTHREAD_RWLOCK_

typedef int pthread_rwlockattr_t;

static int pthread_rwlockattr_destroy(pthread_rwlockattr_t *a)
{
	(void) a;
	return 0;
}

static int pthread_rwlockattr_init(pthread_rwlockattr_t *a)
{
	*a = 0;
}

static int pthread_rwlockattr_getpshared(pthread_rwlockattr_t *a, int *s)
{
	*s = *a;
	return 0;
}

static int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *a, int s)
{
	*a = s;
	return 0;
}

#if TARGET_OS_WIN_XP

typedef pthread_mutex_t pthread_rwlock_t;

static int pthread_rwlock_init(pthread_rwlock_t *l, pthread_rwlockattr_t *a)
{
	return pthread_mutex_init(l);
}

static int pthread_rwlock_destroy(pthread_rwlock_t *l)
{
	return pthread_mutex_destroy(l);
}

static int pthread_rwlock_rdlock(pthread_rwlock_t *l)
{
	return pthread_mutex_lock(l);
}

static int pthread_rwlock_wrlock(pthread_rwlock_t *l)
{
	return pthread_mutex_lock(l);
}

static int pthread_rwlock_unlock(pthread_rwlock_t *l)
{
	return pthread_mutex_unlock(l);
}


static int pthread_rwlock_tryrdlock(pthread_rwlock_t *l)
{
	return pthread_mutex_trylock(l);
}

static int pthread_rwlock_trywrlock(pthread_rwlock_t *l)
{
	return pthread_mutex_trylock(l);
}

static int pthread_rwlock_timedrdlock(pthread_rwlock_t *l, const struct timespec *ts)
{
	return pthread_mutex_timedlock(l, ts);
}

static int pthread_rwlock_timedwrlock(pthread_rwlock_t *l, const struct timespec *ts)
{
	return pthread_mutex_timedlock(l, ts);	
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

typedef SRWLOCK pthread_rwlock_t;

static int pthread_rwlock_init(pthread_rwlock_t *l, pthread_rwlockattr_t *a)
{
	(void) a;
	InitializeSRWLock(l);
	
	return 0;
}

static int pthread_rwlock_destroy(pthread_rwlock_t *l)
{
	(void) *l;
	return 0;
}

static int pthread_rwlock_rdlock(pthread_rwlock_t *l)
{
	pthread_testcancel();
	AcquireSRWLockShared(l);
	
	return 0;
}

static int pthread_rwlock_wrlock(pthread_rwlock_t *l)
{
	pthread_testcancel();
	AcquireSRWLockExclusive(l);
	
	return 0;
}

static int pthread_rwlock_unlock(pthread_rwlock_t *l)
{
	void *state = *(void **)l;
	
	if (state == (void *) 1)
	{
		/* Known to be an exclusive lock */
		ReleaseSRWLockExclusive(l);
	}
	else
	{
		/* A shared unlock will work */
		ReleaseSRWLockShared(l);
	}
	
	return 0;
}


static int pthread_rwlock_tryrdlock(pthread_rwlock_t *l)
{
	/* Get the current state of the lock */
	void *state = *(void **) l;
	
	if (!state)
	{
		/* Unlocked to locked */
		if (!InterlockedCompareExchangePointer((void **) l, (void *)0x11, NULL)) return 0;
		return EBUSY;
	}
	
	/* A single writer exists */
	if (state == (void *) 1) return EBUSY;
	
	/* Multiple writers exist? */
	if ((uintptr_t) state & 14) return EBUSY;
	
	if (InterlockedCompareExchangePointer((void **) l, (void *) ((uintptr_t)state + 16), state) == state) return 0;
	
	return EBUSY;
}

static int pthread_rwlock_trywrlock(pthread_rwlock_t *l)
{
	/* Try to grab lock if it has no users */
	if (!InterlockedCompareExchangePointer((void **) l, (void *)1, NULL)) return 0;
	
	return EBUSY;
}

static int pthread_rwlock_timedrdlock(pthread_rwlock_t *l, const struct timespec *ts)
{
	unsigned long long ct = _pthread_time_in_ms();
	unsigned long long t = _pthread_time_in_ms_from_timespec(ts);

	pthread_testcancel();
	
	/* Use a busy-loop */
	while (1)
	{
		/* Try to grab lock */
		if (!pthread_rwlock_tryrdlock(l)) return 0;
		
		/* Get current time */
		ct = _pthread_time_in_ms();
		
		/* Have we waited long enough? */
		if (ct > t) return ETIMEDOUT;
	}
}

static int pthread_rwlock_timedwrlock(pthread_rwlock_t *l, const struct timespec *ts)
{
	unsigned long long ct = _pthread_time_in_ms();
	unsigned long long t = _pthread_time_in_ms_from_timespec(ts);

	pthread_testcancel();
	
	/* Use a busy-loop */
	while (1)
	{
		/* Try to grab lock */
		if (!pthread_rwlock_trywrlock(l)) return 0;
		
		/* Get current time */
		ct = _pthread_time_in_ms();
		
		/* Have we waited long enough? */
		if (ct > t) return ETIMEDOUT;
	}
}

#endif

#endif /* DISPATCH_WIN_PTHREAD_RWLOCK_ */
