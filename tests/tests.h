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

#ifndef TESTS_H_
#define TESTS_H_

#include "munit/MUnit.h"
#include <xdispatch/dispatch.h>
#include "shims/platform.h"

/* detect wether to build blocks tests or not (the autodetection only works
   safely when building in cpp mode) */
#ifndef XDISPATCH_HAS_BLOCKS
#	if ( defined(MZ_HAS_CXX0X) || defined(__BLOCKS__) || (_MSC_VER >= 1600) )
#		define TEST_BLOCKS
#	endif
#else
#	define TEST_BLOCKS
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* All test cases */

void dispatch_api();
void dispatch_simpleFunction();
void dispatch_apply_function();
void dispatch_priority();
void dispatch_priority2();
void dispatch_after_function();
void dispatch_cascade();
void dispatch_apply_serialqueue();
void dispatch_testDebug();
void test_queue_finalizer();
void dispatch_starfish();
void dispatch_memory_use();
void dispatch_test_sync();
void dispatch_group_function();
#ifdef TEST_BLOCKS
void cross_blocks();
void dispatch_plusplus();
void dispatch_apply_blocks();
void dispatch_after_blocks();
void dispatch_group_blocks();
void dispatch_pingpong_blocks();
void dispatch_semaphore();
void dispatch_timer_bit31();
void dispatch_timer_bit63();
#endif

static void register_tests(){
	MU_REGISTER_TEST(dispatch_api);
	MU_REGISTER_TEST(dispatch_simpleFunction);
	MU_REGISTER_TEST(dispatch_test_sync);
	MU_REGISTER_TEST(dispatch_testDebug);
	MU_REGISTER_TEST(dispatch_apply_function);
	MU_REGISTER_TEST(dispatch_priority);
    MU_REGISTER_TEST(dispatch_priority2);
	MU_REGISTER_TEST(dispatch_cascade);
	MU_REGISTER_TEST(dispatch_apply_serialqueue);
	MU_REGISTER_TEST(dispatch_after_function);
	MU_REGISTER_TEST(test_queue_finalizer);
	MU_REGISTER_TEST(dispatch_starfish);
	MU_REGISTER_TEST(dispatch_memory_use);
	MU_REGISTER_TEST(dispatch_group_function);
#ifdef TEST_BLOCKS
    MU_REGISTER_TEST(cross_blocks);
	MU_REGISTER_TEST(dispatch_plusplus);
	MU_REGISTER_TEST(dispatch_apply_blocks);
	MU_REGISTER_TEST(dispatch_group_blocks);
	MU_REGISTER_TEST(dispatch_after_blocks);
	MU_REGISTER_TEST(dispatch_pingpong_blocks);
	MU_REGISTER_TEST(dispatch_semaphore);
    MU_REGISTER_TEST(dispatch_timer_bit31);
    MU_REGISTER_TEST(dispatch_timer_bit63);
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* TESTS_H_ */
