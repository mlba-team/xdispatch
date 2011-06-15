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

#include <stdlib.h>
#include <xdispatch/dispatch.h>

#include "tests.h"
#include "../core/platform/atomic.h"

/*
	A test for dispatching a function using dispatch_apply_f
	*/

static void argumentFunction(void* data, size_t index){
	dispatch_atomic_inc((intptr_t*)data);
}

void dispatch_apply_function() {
    const unsigned int final = 32;
	intptr_t* count = calloc(1,sizeof(intptr_t));
	dispatch_queue_t  queue;
	MU_BEGIN_TEST(dispatch_apply_function);
	*count = 0;

	queue = dispatch_get_global_queue(0, 0);
	MU_ASSERT_NOT_NULL(queue);

    dispatch_apply_f(final, queue, count, argumentFunction);

	MU_ASSERT_EQUAL(*count, final);
	free(count);

	MU_PASS("Looping works");
	MU_END_TEST
}
