/*
* Copyright (c) 2010 Apple Inc. All rights reserved.
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

#if USE_MACH_SEM
void
_dispatch_semaphore_create_port(semaphore_t *s4);
#endif

// semaphores are too fundamental to use the dispatch_assume*() macros
#if USE_MACH_SEM
#define DISPATCH_SEMAPHORE_VERIFY_KR(x)	do {	\
        if (x) {	\
            DISPATCH_CRASH("flawed group/semaphore logic");	\
        }	\
    } while (0)
#endif
#if USE_POSIX_SEM
#define	DISPATCH_SEMAPHORE_VERIFY_RET(x) do {				\
        if ((x) == -1) {					\
            DISPATCH_CRASH("flawed group/semaphore logic");	\
        }							\
    } while (0)
#endif

static long _dispatch_group_wake(dispatch_semaphore_t dsema);

dispatch_group_t
dispatch_group_create(void)
{
    return (dispatch_group_t)dispatch_semaphore_create(LONG_MAX);
}


DISPATCH_NOINLINE
void
dispatch_group_enter(dispatch_group_t dg)
{
    dispatch_semaphore_t dsema = (dispatch_semaphore_t)dg;
#if USE_APPLE_SEMAPHORE_OPTIMIZATIONS && defined(__OPTIMIZE__) && defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__)) && !defined(__llvm__)
    // This assumes:
    // 1) Way too much about the optimizer of GCC.
    // 2) There will never be more than LONG_MAX threads.
    //    Therefore: no overflow detection
    asm(
#ifdef __LP64__
        "lock decq	%0\n\t"
#else
        "lock decl	%0\n\t"
#endif
        "js 	1f\n\t"
        "ret\n\t"
        "1:"
        : "+m" (dsema->dsema_value)
        :
        : "cc"
    );
    _dispatch_semaphore_wait_slow(dsema, DISPATCH_TIME_FOREVER);
#else
    dispatch_semaphore_wait(dsema, DISPATCH_TIME_FOREVER);
#endif
}

void
dispatch_group_leave(dispatch_group_t dg)
{
    dispatch_semaphore_t dsema = (dispatch_semaphore_t)dg;

    dispatch_semaphore_signal(dsema);

    if (dsema->dsema_value == dsema->dsema_orig) {
        _dispatch_group_wake(dsema);
    }
}

DISPATCH_NOINLINE
long
_dispatch_group_wake(dispatch_semaphore_t dsema)
{
    struct dispatch_sema_notify_s *tmp, *head = dispatch_atomic_xchg(&dsema->dsema_notify_head, NULL);
    long rval = dispatch_atomic_xchg(&dsema->dsema_group_waiters, 0);
    bool do_rel = head;
#if USE_MACH_SEM
    long kr;
#endif
#if USE_POSIX_SEM
    int ret;
#endif

    // wake any "group" waiter or notify blocks

    if (rval) {
#if USE_MACH_SEM
        _dispatch_semaphore_create_port(&dsema->dsema_waiter_port);
        do {
            kr = semaphore_signal(dsema->dsema_waiter_port);
            DISPATCH_SEMAPHORE_VERIFY_KR(kr);
        } while (--rval);
#endif
#if USE_POSIX_SEM
        do {
            ret = sem_post(&dsema->dsema_sem);
            DISPATCH_SEMAPHORE_VERIFY_RET(ret);
        } while (--rval);
#endif
    }
    while (head) {
        dispatch_async_f(head->dsn_queue, head->dsn_ctxt, head->dsn_func);
        _dispatch_release(DO_CAST(head->dsn_queue));
        do {
            tmp = head->dsn_next;
        } while (!tmp && !dispatch_atomic_cmpxchg(&dsema->dsema_notify_tail, head, NULL));
        free(head);
        head = tmp;
    }
    if (do_rel) {
        _dispatch_release(DO_CAST(dsema));
    }
    return 0;
}

DISPATCH_NOINLINE
static long
_dispatch_group_wait_slow(dispatch_semaphore_t dsema, dispatch_time_t timeout)
{
#if USE_MACH_SEM
    mach_timespec_t _timeout;
    kern_return_t kr;
    uint64_t nsec;
#endif
#if USE_POSIX_SEM
    struct timespec _timeout;
    int ret;
#endif
    long orig;

again:
    // check before we cause another signal to be sent by incrementing dsema->dsema_group_waiters
    if (dsema->dsema_value == dsema->dsema_orig) {
        return _dispatch_group_wake(dsema);
    }
    // Mach semaphores appear to sometimes spuriously wake up.  Therefore,
    // we keep a parallel count of the number of times a Mach semaphore is
    // signaled (6880961).
    dispatch_atomic_inc(&dsema->dsema_group_waiters);
    // check the values again in case we need to wake any threads
    if (dsema->dsema_value == dsema->dsema_orig) {
        return _dispatch_group_wake(dsema);
    }

#if USE_MACH_SEM
    _dispatch_semaphore_create_port(&dsema->dsema_waiter_port);
#endif

    // From xnu/osfmk/kern/sync_sema.c:
    // wait_semaphore->count = -1;  /* we don't keep an actual count */
    //
    // The code above does not match the documentation, and that fact is
    // not surprising. The documented semantics are clumsy to use in any
    // practical way. The above hack effectively tricks the rest of the
    // Mach semaphore logic to behave like the libdispatch algorithm.

    switch (timeout) {
    default:
#if USE_MACH_SEM
        do {
            nsec = _dispatch_timeout(timeout);
            _timeout.tv_sec = (typeof(_timeout.tv_sec))(nsec / NSEC_PER_SEC);
            _timeout.tv_nsec = (typeof(_timeout.tv_nsec))(nsec % NSEC_PER_SEC);
            kr = slowpath(semaphore_timedwait(dsema->dsema_waiter_port, _timeout));
        } while (kr == KERN_ABORTED);
        if (kr != KERN_OPERATION_TIMED_OUT) {
            DISPATCH_SEMAPHORE_VERIFY_KR(kr);
            break;
        }
#endif
#if USE_POSIX_SEM
        do {
            _timeout = _dispatch_timeout_ts(timeout);
            ret = slowpath(sem_timedwait(&dsema->dsema_sem,
                &_timeout));
        } while (ret == -1 && errno == EINTR);

        if (!(ret == -1 && errno == ETIMEDOUT)) {
            DISPATCH_SEMAPHORE_VERIFY_RET(ret);
            break;
        }
#endif
        // Fall through and try to undo the earlier change to dsema->dsema_group_waiters
    case DISPATCH_TIME_NOW:
        while ((orig = dsema->dsema_group_waiters)) {
            if (dispatch_atomic_cmpxchg(&dsema->dsema_group_waiters, orig, orig - 1)) {
#if USE_MACH_SEM
                return KERN_OPERATION_TIMED_OUT;
#endif
#if USE_POSIX_SEM
                errno = ETIMEDOUT;
                return -1;
#endif
            }
        }
        // Another thread called semaphore_signal().
        // Fall through and drain the wakeup.
    case DISPATCH_TIME_FOREVER:
#if USE_MACH_SEM
        do {
            kr = semaphore_wait(dsema->dsema_waiter_port);
        } while (kr == KERN_ABORTED);
        DISPATCH_SEMAPHORE_VERIFY_KR(kr);
#endif
#if USE_POSIX_SEM
        do {
            ret = sem_wait(&dsema->dsema_sem);
        } while (ret == -1 && errno == EINTR);
        DISPATCH_SEMAPHORE_VERIFY_RET(ret);
#endif
        break;
    }

    goto again;
}

long
dispatch_group_wait(dispatch_group_t dg, dispatch_time_t timeout)
{
    dispatch_semaphore_t dsema = (dispatch_semaphore_t)dg;

    if (dsema->dsema_value == dsema->dsema_orig) {
        return 0;
    }
    if (timeout == 0) {
#if USE_MACH_SEM
        return KERN_OPERATION_TIMED_OUT;
#endif
#if USE_POSIX_SEM
        errno = ETIMEDOUT;
        return (-1);
#endif
    }
    return _dispatch_group_wait_slow(dsema, timeout);
}

#ifdef __BLOCKS__
void
dispatch_group_notify(dispatch_group_t dg, dispatch_queue_t dq, dispatch_block_t db)
{
    dispatch_group_notify_f(dg, dq, _dispatch_Block_copy(db), _dispatch_call_block_and_release);
}
#endif

void
dispatch_group_notify_f(dispatch_group_t dg, dispatch_queue_t dq, void *ctxt, void (*func)(void *))
{
    dispatch_semaphore_t dsema = (dispatch_semaphore_t)dg;
    struct dispatch_sema_notify_s *dsn, *prev;

    // FIXME -- this should be updated to use the continuation cache
    while (!(dsn = malloc(sizeof(*dsn)))) {
        sleep(1);
    }

    dsn->dsn_next = NULL;
    dsn->dsn_queue = dq;
    dsn->dsn_ctxt = ctxt;
    dsn->dsn_func = func;
    _dispatch_retain(DO_CAST(dq));

    prev = dispatch_atomic_xchg(&dsema->dsema_notify_tail, dsn);
    if (fastpath(prev)) {
        prev->dsn_next = dsn;
    } else {
        _dispatch_retain(DO_CAST(dg));
        dsema->dsema_notify_head = dsn;
        if (dsema->dsema_value == dsema->dsema_orig) {
            _dispatch_group_wake(dsema);
        }
    }
}

#ifdef __BLOCKS__
void
dispatch_group_async(dispatch_group_t dg, dispatch_queue_t dq, dispatch_block_t db)
{
    dispatch_group_async_f(dg, dq, _dispatch_Block_copy(db), _dispatch_call_block_and_release);
}
#endif

DISPATCH_NOINLINE
void
dispatch_group_async_f(dispatch_group_t dg, dispatch_queue_t dq, void *ctxt, void (*func)(void *))
{
    dispatch_continuation_t dc;

    _dispatch_retain(DO_CAST(dg));
    dispatch_group_enter(dg);

    dc = _dispatch_continuation_alloc_cacheonly() ?: _dispatch_continuation_alloc_from_heap();

    dc->do_vtable = (void *)(DISPATCH_OBJ_ASYNC_BIT|DISPATCH_OBJ_GROUP_BIT);
    dc->dc_func = func;
    dc->dc_ctxt = ctxt;
    dc->dc_group = dg;

    _dispatch_queue_push(dq, DO_CAST(dc));
}
