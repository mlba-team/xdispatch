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


#include "../include/xdispatch/dispatch"
#include "cxx_tests.h"

#include <iostream>

#define RUN_TIMES 20
#ifdef XDISPATCH_HAS_BLOCKS

/*
 Little tests mainly checking the correct mapping of the Qt api
 to the underlying C Api
 */

extern "C" void cxx_dispatch_queue(){
    MU_BEGIN_TEST(cxx_dispatch_queue);

	unsigned int* worker = new unsigned int;
	*worker = 0;

    xdispatch::queue* q = XDISPATCH->global_queue(xdispatch::dispatch::HIGH);
	MU_ASSERT_NOT_NULL(q);

    q->apply($(size_t i){
			atomic_inc_get(worker);
    }, RUN_TIMES);

    XDISPATCH->global_queue(xdispatch::dispatch::LOW)->async(${
			MU_ASSERT_EQUAL(*worker,RUN_TIMES);
			MU_PASS("Queue executed");
        });

    XDISPATCH->exec();
	MU_END_TEST;
}

#endif
