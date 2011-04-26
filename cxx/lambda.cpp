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

#include "xdispatch_internal.h"

#if defined(XDISPATCH_HAS_BLOCKS) && !defined(__BLOCKS__)

__XDISPATCH_USE_NAMESPACE

void dispatch_group_async(dispatch_group_t group, dispatch_queue_t queue, dispatch_block_t block){
    dispatch_group_async_f(group,queue, new wrap(block), run_wrap);
}

void dispatch_group_notify(dispatch_group_t group, dispatch_queue_t queue, dispatch_block_t block){
    dispatch_group_notify_f(group,queue, new wrap(block), run_wrap);
}

void dispatch_async(dispatch_queue_t queue, dispatch_block_t block){
    dispatch_async_f(queue, new wrap(block), run_wrap);
}

void dispatch_sync(dispatch_queue_t queue, dispatch_block_t block){
    dispatch_sync_f(queue, new wrap(block), run_wrap);
}

void dispatch_apply(size_t iterations, dispatch_queue_t queue, dispatch_iteration_block_t block){
    dispatch_apply_f(iterations, queue, new iteration_wrap(block,iterations), run_iter_wrap);
}

void dispatch_after(dispatch_time_t when, dispatch_queue_t queue, dispatch_block_t block){
    dispatch_after_f(when, queue, new wrap(block), run_wrap);
}

/* We cannot use a simple wrapper here, as the block might never get executed
   and that way we might produce leaks */

#if defined(__i386__) || defined(__x86_64__)
# define _dispatch_hardware_pause() asm("pause")
#elif _WIN32
# define _dispatch_hardware_pause() __asm pause
#else
# define _dispatch_hardware_pause() asm("")
#endif

void dispatch_once(dispatch_once_t *val, dispatch_block_t block){
	volatile long *vval = val;

	if (atomic_cmpxchg(val, 0l, 1l)) {
		block();

		// The next barrier must be long and strong.
		//
		// The scenario: SMP systems with weakly ordered memory models
		// and aggressive out-of-order instruction execution.
		//
		// The problem:
		//
		// The dispatch_once*() wrapper macro causes the callee's
		// instruction stream to look like this (pseudo-RISC):
		//
		// 	load r5, pred-addr
		// 	cmpi r5, -1
		// 	beq  1f
		// 	call dispatch_once*()
		// 1f:
		// 	load r6, data-addr
		//
		// May be re-ordered like so:
		//
		// 	load r6, data-addr
		// 	load r5, pred-addr
		// 	cmpi r5, -1
		// 	beq  1f
		// 	call dispatch_once*()
		// 1f:
		//
		// Normally, a barrier on the read side is used to workaround
		// the weakly ordered memory model. But barriers are expensive
		// and we only need to synchronize once!  After func(ctxt)
		// completes, the predicate will be marked as "done" and the
		// branch predictor will correctly skip the call to
		// dispatch_once*().
		//
		// A far faster alternative solution: Defeat the speculative
		// read-ahead of peer CPUs.
		//
		// Modern architectures will throw away speculative results
		// once a branch mis-prediction occurs. Therefore, if we can
		// ensure that the predicate is not marked as being complete
		// until long after the last store by func(ctxt), then we have
		// defeated the read-ahead of peer CPUs.
		//
		// In other words, the last "store" by func(ctxt) must complete
		// and then N cycles must elapse before ~0l is stored to *val.
		// The value of N is whatever is sufficient to defeat the
		// read-ahead mechanism of peer CPUs.
		//
		// On some CPUs, the most fully synchronizing instruction might
		// need to be issued.
	
		atomic_barrier();
		*val = ~0l;
	} else {
		do {
			_dispatch_hardware_pause();
		} while (*vval != ~0l);

		atomic_barrier();
	}
}

#endif
