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


#include <xdispatch/dispatch>
#include "cxx_tests.h"

#include <iostream>

#define RUN_TIMES 20

/*
 Little tests mainly checking the correct mapping of the Qt api
 to the underlying C Api
 */

extern "C" void cxx_dispatch_queue_blocks(){
    MU_BEGIN_TEST(cxx_dispatch_queue_blocks);

	uintptr_t* worker = new uintptr_t;
	*worker = 0;

    xdispatch::queue q = xdispatch::global_queue(xdispatch::HIGH);
    MU_ASSERT_NOT_NULL(q.native());

    q.apply(^(size_t i){
        dispatch_atomic_inc(worker);
    }, RUN_TIMES);

    xdispatch::global_queue(xdispatch::LOW).async(^{
        MU_ASSERT_EQUAL(*worker,RUN_TIMES);
        MU_PASS("Queue executed");
    });

    xdispatch::exec();
	MU_END_TEST;
}
