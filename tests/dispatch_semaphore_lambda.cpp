/*
* Copyright (c) 2008-2009 Apple Inc. All rights reserved.
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

#include <stdio.h>
#include <xdispatch/dispatch.h>
#include <stdlib.h>
#include <assert.h>

#include "tests.h"

#define LAPS 10000


extern "C" void dispatch_semaphore_lambda()
{
	static size_t total;
	dispatch_semaphore_t dsema;

    MU_BEGIN_TEST(dispatch_semaphore_lambda);

	dsema = dispatch_semaphore_create(1);
    MU_ASSERT_NOT_NULL(dsema);

    dispatch_apply(LAPS, dispatch_queue_create(0,0), [=](size_t idx) {
		dispatch_semaphore_wait(dsema, DISPATCH_TIME_FOREVER);
		total++;
		dispatch_semaphore_signal(dsema);
	});

	dispatch_release(dsema);

	MU_ASSERT_EQUAL(total, LAPS);

	MU_PASS("");
	MU_END_TEST;
}
