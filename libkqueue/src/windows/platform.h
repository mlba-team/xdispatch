/*
 * Copyright (c) 2011 Mark Heily <mark@heily.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef  _KQUEUE_WINDOWS_PLATFORM_H
#define  _KQUEUE_WINDOWS_PLATFORM_H

/* Reduces build time by omitting extra system headers */
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
 
#define _CRT_SECURE_NO_WARNINGS 1
/* The #define doesn't seem to work, but the #pragma does.. */
#ifdef _MSC_VER
# pragma warning( disable : 4996 )
#endif

#include "../../include/sys/event.h"


/*
 * Atomic integer operations 
 */
#ifdef __GNUC__
# define atomic_inc(p)   __sync_add_and_fetch((p), 1)
# define atomic_dec(p)   __sync_sub_and_fetch((p), 1)
# define atomic_cas(p, oval, nval) __sync_val_compare_and_swap(p, oval, nval)
# define atomic_ptr_cas(p, oval, nval) __sync_val_compare_and_swap(p, oval, nval)
#else
# define atomic_inc   InterlockedIncrement
# define atomic_dec   InterlockedDecrement
# define atomic_cas(p, oval, nval) InterlockedCompareExchange(p, nval, oval)
# define atomic_ptr_cas(p, oval, nval) InterlockedCompareExchangePointer(p, nval, oval)
#endif

/*
 * Additional members of struct kqueue
 * FIXME: This forces a thread-per-filter model
 *			Would be better to 
 */
#define KQUEUE_PLATFORM_SPECIFIC \
	HANDLE kq_handle; \
	HANDLE kq_apc_dispatcher; \
    HANDLE kq_filt_handle[EVFILT_SYSCOUNT]; \
	struct filter *kq_filt_ref[EVFILT_SYSCOUNT]; \
    size_t kq_filt_count; \
	DWORD  kq_filt_signalled

/*
 * Additional members of struct filter
 */
#define FILTER_PLATFORM_SPECIFIC \
	HANDLE kf_event_handle

/*
 * Some datatype forward declarations
 */
struct filter;

/*
 * Hooks and prototypes
 */
int     windows_kqueue_init(struct kqueue *);
void    windows_kqueue_free(struct kqueue *);
int     windows_kevent_wait(struct kqueue *, int, const struct timespec *);
int     windows_kevent_copyout(struct kqueue *, int, struct kevent *, int);
int     windows_filter_init(struct kqueue *, struct filter *);
void    windows_filter_free(struct kqueue *, struct filter *);

/* Windows does not support this attribute.
   DllMain() is the only available constructor function.
   This means the constructor must be called from within DllMain().
 */
#define CONSTRUCTOR

/*
 * GCC-compatible branch prediction macros
 */
#ifdef __GNUC__
# define fastpath(x)     __builtin_expect((x), 1)
# define slowpath(x)     __builtin_expect((x), 0)
#else
# define fastpath(x) (x)
# define slowpath(x) (x)
#endif

/* Function visibility macros */
#define VISIBLE __declspec(dllexport)
#define HIDDEN  

#if !defined(__func__) && !defined(__GNUC__)
#define __func__ __FUNCDNAME__
#endif

#define snprintf _snprintf
#define ssize_t  SSIZE_T
#define sleep(x) Sleep((x) * 1000)
#define inline __inline

/* For POSIX compatibility when compiling, not for actual use */
typedef int socklen_t;
typedef int nlink_t;
typedef int timer_t;
typedef int pthread_t;
typedef int sigset_t;
typedef int pid_t;

#define __thread    __declspec(thread)

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
#define pthread_spin_lock _cs_lock
#define pthread_spin_unlock _cs_unlock
#define pthread_spin_init(x,y) _cs_init((x))
#define pthread_mutex_init(x,y) _cs_init((x))
#define pthread_rwlock_rdlock _cs_lock
#define pthread_rwlock_wrlock _cs_lock
#define pthread_rwlock_unlock _cs_unlock
#define pthread_rwlock_init(x,y) _cs_init((x))

#endif  /* ! _KQUEUE_WINDOWS_PLATFORM_H */
