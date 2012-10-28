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

#include "tests.h"
#include <xdispatch/dispatch.h>

#ifdef XDISPATCH_HAS_BLOCKS

/*
  A small test for checking the blocks functionality
  */

static int result;
static dispatch_block_t myTest;

void installBlock(){
	int a = 3, b = 4;
    dispatch_block_t block = (${
		result = a*b;
    });

    myTest = Block_copy(block);
}

extern "C" void cross_blocks(){
	MU_BEGIN_TEST(cross_blocks);

	installBlock();
    myTest();
	MU_ASSERT_EQUAL(result,12);
	MU_PASS("Blocks are running");

    Block_release(myTest);

	MU_END_TEST;
}

#endif
