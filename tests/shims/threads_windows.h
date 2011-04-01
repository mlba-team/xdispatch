/*
 * Copyright (c) 2010 Apple Inc. All rights reserved.
 * Copyright (c) 2011 MLBA. All rights reserved.
 * Copyright (c) 2010 Mark Heily <mark@heily.com>
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

/*
 * IMPORTANT: This header file describes INTERNAL interfaces to libdispatch
 * which are subject to change in future releases of Mac OS X. Any applications
 * relying on these interfaces WILL break.
 */

#ifndef __DISPATCH_SHIMS_TSD_WINDOWS__
#define __DISPATCH_SHIMS_TSD_WINDOWS__

/* threads */

typedef HANDLE pthread_t;
typedef DWORD pthread_attr_t;

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    void *(*start_routine)(void*), void *arg);

/* Emulation of pthreads mutex functionality */
#define PTHREAD_PROCESS_SHARED 1
#define PTHREAD_PROCESS_PRIVATE 2
typedef CRITICAL_SECTION pthread_mutex_t;
typedef CRITICAL_SECTION pthread_spinlock_t;
typedef CRITICAL_SECTION pthread_rwlock_t;
#define _cs_init(x)  InitializeCriticalSection((x))
#define _cs_lock(x)  EnterCriticalSection ((x))
#define _cs_unlock(x)  LeaveCriticalSection ((x))
#define pthread_mutex_lock _cs_lock
#define pthread_mutex_unlock _cs_unlock
#define pthread_mutex_init(x,y) _cs_init((x))
#define pthread_mutex_destroy(x)
#define pthread_spin_lock _cs_lock
#define pthread_spin_unlock _cs_unlock
#define pthread_spin_init(x,y) _cs_init((x))
#define pthread_rwlock_rdlock _cs_lock
#define pthread_rwlock_wrlock _cs_lock
#define pthread_rwlock_unlock _cs_unlock
#define pthread_rwlock_init(x,y) _cs_init((x))

/* conditions */
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

int pthread_cond_init (pthread_cond_t *cv, const pthread_condattr_t *);
int pthread_cond_wait (pthread_cond_t *cv, pthread_mutex_t *external_mutex);
int pthread_cond_signal (pthread_cond_t *cv);
int pthread_cond_broadcast (pthread_cond_t *cv);
int pthread_cond_destroy(pthread_cond_t *cv);

/* tsd */
typedef DWORD pthread_key_t;
#define pthread_key_create(k,d) (!((*k=TlsAlloc())!=TLS_OUT_OF_INDEXES))
#define pthread_setspecific(k,v) (!(TlsSetValue(k, v)))
#define pthread_getspecific(k) (TlsGetValue(k))

/* semaphores */
typedef HANDLE sem_t;

int sem_init(sem_t *, int, unsigned);
#define sem_destroy(s) CloseHandle((s))
#define sem_post(s) (!(ReleaseSemaphore((s),1,0)))
#define sem_wait(s) ((WaitForSingleObject((s),INFINITE))==WAIT_FAILED)
int sem_timedwait(sem_t *, const struct timespec *);

/* custom dispatch code */

extern pthread_key_t dispatch_queue_key;
extern pthread_key_t dispatch_sema4_key;
extern pthread_key_t dispatch_cache_key;
extern pthread_key_t dispatch_bcounter_key;

static inline void
_dispatch_thread_setspecific(pthread_key_t k, void *v)
{

    dispatch_assert(TlsSetValue(k, v));

}

static inline void *
_dispatch_thread_getspecific(pthread_key_t k)
{

    return TlsGetValue(k);
}

static inline void
_dispatch_thread_key_create(pthread_key_t *key, void (*destructor)(void *))
{
    // TODO: What about the destructor in here?
    *key = TlsAlloc();
    dispatch_assert( (*key) != TLS_OUT_OF_INDEXES);
}

//#define _dispatch_thread_self (uintptr_t)pthread_self

#endif /* __DISPATCH_SHIMS_TSD_WINDOWS__ */
