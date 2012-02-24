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

#include <stdio.h>
#include <xdispatch/dispatch.h>
#include <stdlib.h>
#include <assert.h>

#include "../core/platform/atomic.h"
#include "tests.h"

/*
 A test controlling that the different priorities
 really get different treatment
 */

static intptr_t done = 0;

#define BLOCKS 128
#define PRIORITIES 3

#if TARGET_OS_EMBEDDED
#define LOOP_COUNT 2000000
#else
#define LOOP_COUNT 20000000
#endif

char *labels[PRIORITIES] = { "LOW", "DEFAULT", "HIGH" };
int priorities[PRIORITIES] = { DISPATCH_QUEUE_PRIORITY_LOW, DISPATCH_QUEUE_PRIORITY_DEFAULT, DISPATCH_QUEUE_PRIORITY_HIGH };

union {
	size_t count;
	char padding[64];
} counts[PRIORITIES];

#define ITERATIONS (size_t)(PRIORITIES * BLOCKS * 0.5)
static uintptr_t iterations = ITERATIONS;

static void histogram(void) {
    size_t maxcount = BLOCKS;
	size_t sc[PRIORITIES];
	size_t total = 0;

    size_t x, y;
    for (y = 0; y < PRIORITIES; ++y) {
		sc[y] = counts[y].count;
	}

	for (y = 0; y < PRIORITIES; ++y) {
		double fraction = 0, value = 0;
		MU_MESSAGE("%s: %ld",labels[y], sc[y]);
		total += sc[y];

        fraction = (double)sc[y] / (double)maxcount;
        value = fraction * (double)80;
        for (x = 0; x < 80; ++x) {
            printf("%s", (value > x) ? "*" : " ");
        }
        printf("\n");
	}
	
	MU_ASSERT_EQUAL(total, ITERATIONS);
	MU_ASSERT_TRUE((long)sc[0]<=(long)sc[2]);
	MU_PASS("Please check histogram to be really sure");
}

void
cpubusy(void* context)
{
	uintptr_t *count = (uintptr_t*)context;
	uintptr_t iterdone;

	size_t idx;
	for (idx = 0; idx < LOOP_COUNT; ++idx) {
		if (done) return;
	}
	
	iterdone = dispatch_atomic_dec(&iterations);
	if (iterdone==0) {
		dispatch_atomic_inc(&done);
		dispatch_atomic_inc(count);
		histogram();
	} else if (iterdone > 0) {
		dispatch_atomic_inc(count);
	}
}

static void
submit_work(dispatch_queue_t queue, void* context)
{
	int i;

	for (i = 0; i < BLOCKS; ++i) {
		dispatch_async_f(queue, context, cpubusy);
	}

#if USE_SET_TARGET_QUEUE
	dispatch_release(queue);
#endif
}

void dispatch_priority()
{
	dispatch_queue_t q[PRIORITIES];
	int i;

	MU_BEGIN_TEST(dispatch_priority);
	for(i = 0; i < PRIORITIES; i++) {
		q[i] = dispatch_get_global_queue(priorities[i], 0);
	}

	for(i = 0; i < PRIORITIES; i++) {
		submit_work(q[i], &counts[i].count);
	}

	dispatch_main();

	MU_FAIL("Should never reach this");
	MU_END_TEST;
}

void dispatch_priority2()
{
	dispatch_queue_t q[PRIORITIES];
	int i;

	MU_BEGIN_TEST(dispatch_priority2);
	for(i = 0; i < PRIORITIES; i++) {
		q[i] = dispatch_queue_create(labels[i], NULL);
		MU_ASSERT_NOT_NULL(q[i]);
        MU_ASSERT_NULL(q[i]);
		dispatch_set_target_queue(q[i], dispatch_get_global_queue(priorities[i], 0));
		// TODO: do we need this?
		//dispatch_queue_set_width(q[i], -2);
	}
	
	for(i = 0; i < PRIORITIES; i++) {
		submit_work(q[i], &counts[i].count);
	}

	dispatch_main();

	MU_FAIL("Should never reach this");
	MU_END_TEST;
}
