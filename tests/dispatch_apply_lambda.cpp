/*
* dispatch_apply_lambda.cpp
* 
* Copyright (c) 2008-2009 Apple Inc.
* Copyright (c) 2011-2013 MLBA-Team.
* All rights reserved.
* 
* @LICENSE_HEADER_START@
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
* @LICENSE_HEADER_END@
*/
#include <xdispatch/dispatch.h>


#include "tests.h"

/*
	A test for dispatching a block using dispatch_apply
	*/

extern "C" void dispatch_apply_lambda() {
	uintptr_t* count = new uintptr_t;
	*count = 0;
	const unsigned int final = 32;

    MU_BEGIN_TEST(dispatch_apply_lambda);
	dispatch_queue_t queue = dispatch_get_global_queue(0, 0);
	MU_ASSERT_NOT_NULL(queue);

    dispatch_apply(final, queue, [=](size_t i) {
		dispatch_atomic_inc(count);
	});

	MU_ASSERT_EQUAL(*count, final);
	delete count;

	MU_PASS("Looping works for blocks");
	MU_END_TEST;
}

