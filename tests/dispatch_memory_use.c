/*
* Copyright (c) 2011-2012 MLBA-Team. All rights reserved.
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
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>
#else
#	include <unistd.h>
#endif

#include "../core/platform/atomic.h"
#include "tests.h"

#if TARGET_OS_EMBEDDED
#define COUNT	300ul
#define QUEUES	4ul
#else
#define COUNT	30000ul
#define QUEUES	100ul
#endif

static dispatch_queue_t queues[QUEUES];


static void collect(void *context)
{
	
	MU_MESSAGE("All workers done.");

	// give the threads some time to settle before test_stop() runs "leaks"
	// ...also note, this is a total cheat.   dispatch_after lets this
	// thread go idle, so dispatch cleans up the continuations cache.
	// Doign the "old style" sleep left that stuff around and leaks
    // took a LONG TIME to complete. Long enough that the test harness
	// decided to kill us.
	dispatch_after_f(dispatch_time(DISPATCH_TIME_NOW, 2 * NSEC_PER_SEC), dispatch_get_main_queue(), NULL, MU_PASS_AFTER_DELAY);
}

static void work(void* context){
//#ifdef WIN32
//		Sleep(1);
//#else
//		usleep(1*1000);
//#endif
}

static void node_done(void* context){
	//MU_MESSAGE("Node %s done", dispatch_queue_get_label(dispatch_get_current_queue()));
}

static void
		start_node(void *context)
{
	dispatch_group_t g = (dispatch_group_t)(context);
	size_t i;
	dispatch_queue_t this_q = dispatch_get_current_queue();

	for (i = 0; i < COUNT; i++) {
		dispatch_group_async_f(g, this_q, NULL, work);
	}

	dispatch_group_async_f(g, this_q, NULL, node_done);

	//MU_MESSAGE("Spawned node %s", dispatch_queue_get_label(dispatch_get_current_queue()));
}

static void do_test(void)
{
	size_t i;
	char buf[1000];
	dispatch_group_t g = dispatch_group_create();

	for (i = 0; i < QUEUES; i++) {
#ifdef WIN32
		_snprintf(buf, sizeof(buf), "com.example.memoryload-node#%ld", i);
#else
		snprintf(buf, sizeof(buf), "com.example.memoryload-node#%ld", (long int)i);
#endif
		queues[i] = dispatch_queue_create(buf, NULL);
		dispatch_suspend(queues[i]);
	}

	for (i = 0; i < QUEUES; i++) {
		dispatch_group_async_f(g, queues[i], g, start_node);
	}

	dispatch_group_notify_f(g, dispatch_get_main_queue(), NULL, collect);

	for (i = 0; i < QUEUES; i++) {
		dispatch_resume(queues[i]);
		dispatch_release(queues[i]);
	}
}

void dispatch_memory_use() {
	MU_BEGIN_TEST(dispatch_memory_use);

	do_test();

	dispatch_main();

	MU_FAIL("Should never reach this");
	MU_END_TEST
}
