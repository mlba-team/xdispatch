/*
* Copyright (c) 2008-2009 Apple Inc. All rights reserved.
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

#define __DISPATCH_QUEUE_INCLUDED
# include "core.c"
#undef __DISPATCH_QUEUE_INCLUDED

dispatch_queue_t
dispatch_get_current_queue(void)
{
    return _dispatch_queue_get_current() ? _dispatch_queue_get_current()  : _dispatch_get_root_queue(0, true);
}

dispatch_queue_t
dispatch_get_main_queue(void)
{
    return &_dispatch_main_q;
}

#ifdef __BLOCKS__
void
dispatch_sync(dispatch_queue_t dq, void (^work)(void))
{
    struct Block_basic *bb = (void *)work;
    dispatch_sync_f(dq, work, (dispatch_function_t)bb->Block_invoke);
}
#endif

DISPATCH_NOINLINE
void
dispatch_sync_f(dispatch_queue_t dq, void *ctxt, dispatch_function_t func)
{
    typeof(dq->dq_running) prev_cnt;
    dispatch_queue_t old_dq;

    if (dq->dq_width == 1) {
        return dispatch_barrier_sync_f(dq, ctxt, func);
    }

    // 1) ensure that this thread hasn't enqueued anything ahead of this call
    // 2) the queue is not suspended
    if (slowpath(dq->dq_items_tail) || slowpath(DISPATCH_OBJECT_SUSPENDED(dq))) {
        _dispatch_sync_f_slow(dq);
    } else {
        prev_cnt = dispatch_atomic_add(&dq->dq_running, 2) - 2;

        if (slowpath(prev_cnt & 1)) {
            if (dispatch_atomic_sub(&dq->dq_running, 2) == 0) {
                _dispatch_wakeup(dq);
            }
            _dispatch_sync_f_slow(dq);
        }
    }

    old_dq = _dispatch_thread_getspecific(dispatch_queue_key);
    _dispatch_thread_setspecific(dispatch_queue_key, dq);
    func(ctxt);
    _dispatch_workitem_inc();
    _dispatch_thread_setspecific(dispatch_queue_key, old_dq);

    if (slowpath(dispatch_atomic_sub(&dq->dq_running, 2) == 0)) {
        _dispatch_wakeup(dq);
    }
}

const char *
dispatch_queue_get_label(dispatch_queue_t dq)
{
    return dq->dq_label;
}

void
dispatch_main(void)
{

#if HAVE_PTHREAD_MAIN_NP
    if (pthread_main_np()) {
#endif
        _dispatch_program_is_probably_callback_driven = true;
        pthread_exit(NULL);
        DISPATCH_CRASH("pthread_exit() returned");
#if HAVE_PTHREAD_MAIN_NP
    }
    DISPATCH_CLIENT_CRASH("dispatch_main() must be called on the main thread");
#endif
}

dispatch_queue_t
dispatch_get_global_queue(long priority, unsigned long flags)
{
    if (flags & ~DISPATCH_QUEUE_OVERCOMMIT) {
        return NULL;
    }
    return _dispatch_get_root_queue(priority, flags & DISPATCH_QUEUE_OVERCOMMIT);
}

void
dispatch_set_target_queue(dispatch_object_t dou, dispatch_queue_t dq)
{
    if (slowpath(DO_CAST(dou)->do_xref_cnt == DISPATCH_OBJECT_GLOBAL_REFCNT)) {
        return;
    }
    // NOTE: we test for NULL target queues internally to detect root queues
    // therefore, if the retain crashes due to a bad input, that is OK
    _dispatch_retain(DO_CAST(dq));
    dispatch_barrier_async_f(DQUEUE_CAST(dou), dq, _dispatch_set_target_queue2);
}

#ifdef __BLOCKS__
void
dispatch_after(dispatch_time_t when, dispatch_queue_t queue, dispatch_block_t work)
{
    // test before the copy of the block
    if (when == DISPATCH_TIME_FOREVER) {
#if DISPATCH_DEBUG
        DISPATCH_CLIENT_CRASH("dispatch_after() called with 'when' == infinity");
#endif
        return;
    }
    dispatch_after_f(when, queue, _dispatch_Block_copy(work), _dispatch_call_block_and_release);
}
#endif

struct _dispatch_after_time_s {
    void *datc_ctxt;
    void (*datc_func)(void *);
    dispatch_source_t ds;
};

static void
_dispatch_after_timer_cancel(void *ctxt)
{
    struct _dispatch_after_time_s *datc = ctxt;
    dispatch_source_t ds = datc->ds;

    free(datc);
    dispatch_release(ds);	// MUST NOT be _dispatch_release()
}

static void
_dispatch_after_timer_callback(void *ctxt)
{
    struct _dispatch_after_time_s *datc = ctxt;

    dispatch_assert(datc->datc_func);
    datc->datc_func(datc->datc_ctxt);

    dispatch_source_cancel(datc->ds);
}

DISPATCH_NOINLINE
void
dispatch_after_f(dispatch_time_t when, dispatch_queue_t queue, void *ctxt, void (*func)(void *))
{
    uint64_t delta;
    struct _dispatch_after_time_s *datc = NULL;
    dispatch_source_t ds = NULL;

    if (when == DISPATCH_TIME_FOREVER) {
#if DISPATCH_DEBUG
        DISPATCH_CLIENT_CRASH("dispatch_after_f() called with 'when' == infinity");
#endif
        return;
    }

    delta = _dispatch_timeout(when);
    if (delta == 0) {
        return dispatch_async_f(queue, ctxt, func);
    }

    // this function should be optimized to not use a dispatch source
    ds = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, queue);
    dispatch_assert(ds);

    datc = malloc(sizeof(struct _dispatch_after_time_s));
    dispatch_assert(datc);
    datc->datc_ctxt = ctxt;
    datc->datc_func = func;
    datc->ds = ds;

    dispatch_set_context(ds, datc);
    dispatch_source_set_event_handler_f(ds, _dispatch_after_timer_callback);
    dispatch_source_set_cancel_handler_f(ds, _dispatch_after_timer_cancel);
    dispatch_source_set_timer(ds, when, 0, 0);
    dispatch_resume(ds);
}
