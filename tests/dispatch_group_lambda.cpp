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
#endif

#include "tests.h"

#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC 1000000000
#endif

static dispatch_group_t create_group(size_t count, int delay) {
	size_t i;

	dispatch_group_t group = dispatch_group_create();

	for (i = 0; i < count; ++i) {
		dispatch_queue_t queue = dispatch_queue_create(NULL, NULL);
		MU_ASSERT_NOT_NULL(queue);

        dispatch_group_async(group, queue, [=]{
			if (delay) {
				MU_MESSAGE("sleeping...");
#ifdef WIN32
				// on windows, sleep is in ms
				Sleep(delay*1000);
#else
				sleep(delay);
#endif
				MU_MESSAGE("done.");
			}
		});

		dispatch_release(queue);
	}
	return group;
}

extern "C"
void dispatch_group_lambda() {
	long res;
	dispatch_group_t group;

    MU_BEGIN_TEST(dispatch_group_lambda);

	group = create_group(100, 0);
	MU_ASSERT_NOT_NULL(group);

	dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
	
	// should be OK to re-use a group
    dispatch_group_async(group, dispatch_get_global_queue(0, 0), [=]{});
	dispatch_group_wait(group, DISPATCH_TIME_FOREVER);

	dispatch_release(group);
	group = NULL;
	
	group = create_group(3, 7);
	MU_ASSERT_NOT_NULL(group);

	res = dispatch_group_wait(group, dispatch_time(DISPATCH_TIME_NOW, 5ull * NSEC_PER_SEC));
    MU_ASSERT_EQUAL(!res, 0);

	// retry after timeout (this time succeed)
	res = dispatch_group_wait(group, dispatch_time(DISPATCH_TIME_NOW, 5ull * NSEC_PER_SEC));
    MU_ASSERT_EQUAL(res, 0);

	dispatch_release(group);
	group = NULL;

	group = create_group(100, 0);
	MU_ASSERT_NOT_NULL(group);

    dispatch_group_notify(group, dispatch_get_main_queue(), [=]{
		dispatch_queue_t m = dispatch_get_main_queue();
		dispatch_queue_t c = dispatch_get_current_queue();
		MU_ASSERT_EQUAL(m, c);
		MU_PASS("");
	});
	
	dispatch_release(group);
	group = NULL;

	dispatch_main();

	MU_FAIL("Should never reach this");
	MU_END_TEST
}

