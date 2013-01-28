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

#include <xdispatch/dispatch.h>


#include "tests.h"

/*
 A short test for ensuring that dispatch a block
 in C++ is working
 */

extern "C" void dispatch_plusplus_blocks(void) {
    MU_BEGIN_TEST(dispatch_plusplus_blocks);

	dispatch_queue_t q = dispatch_get_main_queue();
	MU_ASSERT_NOT_NULL(q);

	dispatch_async(dispatch_get_main_queue(), ^{
        int i = 0;
        i++;
        MU_ASSERT_EQUAL(i, 1);
        
        const char* pass = "Dispatching blocks works";
		MU_PASS(pass);
	});
	dispatch_main();
	MU_FAIL("Should never reach this");
	MU_END_TEST
}
