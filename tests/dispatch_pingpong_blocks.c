/*
* Copyright (c) 2008-2009 Apple Inc. All rights reserved.
* Copyright (c) 2011-2013 MLBA-Team. All rights reserved.
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
#include <string.h>

#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN 1
#	include <windows.h>
#else
#	include <unistd.h>
#	include <time.h>
#endif


#include "tests.h"


uintptr_t count = 0;
const uintptr_t final = 10000; // 10M

static void pingpongloop(dispatch_group_t group, dispatch_queue_t ping, dispatch_queue_t pong, size_t counter) {
	//printf("[%p] %s: %lu\n", (void*)(uintptr_t)pthread_self(), dispatch_queue_get_label(dispatch_get_current_queue()), counter);
	if (counter < final) {
		dispatch_group_async(group, pong, ^{ pingpongloop(group, pong, ping, counter+1); });
	} else {
		count = counter;
	}
}

void dispatch_pingpong_blocks() {
	dispatch_group_t group;
	dispatch_queue_t ping, pong;

	MU_BEGIN_TEST(dispatch_pingpong_blocks);

	ping = dispatch_queue_create("ping", NULL);
	MU_ASSERT_NOT_NULL(ping);
	pong = dispatch_queue_create("pong", NULL);
	MU_ASSERT_NOT_NULL(pong);
	
	group = dispatch_group_create();
	MU_ASSERT_NOT_NULL(group);
	
	pingpongloop(group, ping, pong, 0);
	dispatch_group_wait(group, DISPATCH_TIME_FOREVER);

	MU_ASSERT_EQUAL(count, final);
	MU_PASS("");

	MU_END_TEST
}

