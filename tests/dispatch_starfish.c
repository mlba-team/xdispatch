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

// windows warns because of _snprintf :/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <xdispatch/dispatch.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#ifdef WIN32
#	include <windows.h>
#else
#	include <unistd.h>
#endif

#include "../core/shim/atomic.h"
#include "tests.h"

#if TARGET_OS_EMBEDDED
#define COUNT	300ul
#define LAPS	10ul
#else
#define COUNT	300ul
#define LAPS	10ul
#endif

static dispatch_queue_t queues[COUNT];
static size_t lap_count_down = LAPS;
static size_t count_down;
static uint64_t start;


static void do_test(void);

static void
		collect(void *context)
{
	uint64_t delta;
	long double math;
	size_t i;

	if (--count_down) {
		return;
	}

	delta = dispatch_time(0,0) - start;
	math = (long double)delta;
	math /= COUNT * COUNT * 2ul + COUNT * 2ul;

	MU_MESSAGE("lap: %ld", lap_count_down);
	MU_MESSAGE("count: %lu", COUNT);
	MU_MESSAGE("delta: %llu ns", (uintmax_t)delta);
	MU_MESSAGE("math: %Lf ns / lap", math);

	for (i = 0; i < COUNT; i++) {
		dispatch_release(queues[i]);
	}

	// our malloc could be a lot better,
	// this result is really a malloc torture test
	MU_ASSERT_TRUE((unsigned long)math<10000);

	if (--lap_count_down) {
		do_test();
		return;
	}

	// give the threads some time to settle before test_stop() runs "leaks"
	// ...also note, this is a total cheat.   dispatch_after lets this
	// thread go idle, so dispatch cleans up the continuations cache.
	// Doign the "old style" sleep left that stuff around and leaks
	// took a LONG TIME to complete.   Long enough that the test harness
	// decided to kill us.
	dispatch_after_f(dispatch_time(DISPATCH_TIME_NOW, 2 * NSEC_PER_SEC), dispatch_get_main_queue(), NULL, MU_PASS_AFTER_DELAY);
}

static void
		pong(void *context)
{
	dispatch_queue_t this_q = (dispatch_queue_t)context;
	size_t replies = (size_t)dispatch_get_context(this_q);

	dispatch_set_context(this_q, (void *)--replies);
	if (!replies) {
		//MU_MESSAGE("collect from: %s\n", dispatch_queue_get_label(dispatch_get_current_queue()));
		dispatch_async_f(dispatch_get_main_queue(), NULL, collect);
	}
}

static void
		ping(void *context)
{
	dispatch_queue_t reply_q = (dispatch_queue_t)context;

	dispatch_async_f(reply_q, reply_q, pong);
}

static void
		start_node(void *context)
{
	dispatch_queue_t this_q = (dispatch_queue_t)context;
	size_t i;

	dispatch_set_context(this_q, (void *)COUNT);

	for (i = 0; i < COUNT; i++) {
		dispatch_async_f(queues[i], this_q, ping);
	}
}

static void do_test(void)
{
	size_t i;
	char buf[1000];

	count_down = COUNT;

	start = dispatch_time(0,0);

	for (i = 0; i < COUNT; i++) {
#ifdef WIN32
		_snprintf(buf, sizeof(buf), "com.example.starfish-node#%ld", i);
#else
		snprintf(buf, sizeof(buf), "com.example.starfish-node#%ld", (long int)i);
#endif
		queues[i] = dispatch_queue_create(buf, NULL);
		dispatch_suspend(queues[i]);
	}

	for (i = 0; i < COUNT; i++) {
		dispatch_async_f(queues[i], queues[i], start_node);
	}

	for (i = 0; i < COUNT; i++) {
		dispatch_resume(queues[i]);
	}
}

void dispatch_starfish() {
	MU_BEGIN_TEST(dispatch_starfish);

	do_test();

	dispatch_main();

	MU_FAIL("Should never reach this");
	MU_END_TEST
}
