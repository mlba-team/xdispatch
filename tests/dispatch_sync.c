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

#include "tests.h"

#include <xdispatch/dispatch.h>

/*
 A simple test for ensuring that the sync call really
 only returns if the dispatched object was executed
 */

int number = 0;

static void work(void *context)
{
	number = 4;
}

void dispatch_test_sync() {
	dispatch_queue_t q;

	MU_BEGIN_TEST(dispatch_sync);

	q = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
	MU_ASSERT_NOT_NULL(q);

	dispatch_sync_f(q, NULL, work);
	MU_ASSERT_EQUAL(number, 4);

	MU_PASS("Queue order can't be completely wrong");
	MU_END_TEST;
}
