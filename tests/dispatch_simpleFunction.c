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

#include <xdispatch/dispatch.h>
#include <stdlib.h>

#include "tests.h"

/*
 A simple test checking wether the dispatching
 of a simple function is correctly processed
 */

static void work(void *context)
{
	MU_PASS("Scheduled function was called");
}

void dispatch_simpleFunction(void) {
	dispatch_queue_t q;
	MU_BEGIN_TEST(dispatch_simpleFunction);
	q = dispatch_get_main_queue();
	MU_ASSERT_NOT_NULL(q);

	dispatch_async_f(dispatch_get_main_queue(), NULL, work);
	dispatch_main();
	MU_FAIL("Should never reach this");
	MU_END_TEST;
}
