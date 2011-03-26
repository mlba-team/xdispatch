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

#ifndef CXX_TESTS_H_
#define CXX_TESTS_H_

#include "munit/MUnit.h"
#include "../core/atomic.h"

#ifdef __cplusplus
# include <xdispatch/dispatch>
#endif


/* detect wether to build blocks tests or not (the autodetection only works
   safely when building in cpp mode) */
#ifndef XDISPATCH_HAS_BLOCKS
#	if (defined __GXX_EXPERIMENTAL_CXX0X__ || defined __BLOCKS__ || _MSC_VER >= 1600)
#		define TEST_BLOCKS
#	endif
#else
#	define TEST_BLOCKS
#endif

#ifdef __cplusplus
extern "C" {
#endif

void cxx_dispatch_debug();
void cxx_dispatch_cascade();
void cxx_dispatch_group();
void cxx_dispatch_mainqueue();
void cxx_dispatch_queue();
void cxx_dispatch_serialqueue();
void cxx_dispatch_semaphore();
void cxx_dispatch_current();

static void register_cxx_tests(){
    MU_REGISTER_TEST(cxx_dispatch_debug);
    MU_REGISTER_TEST(cxx_dispatch_queue);
    MU_REGISTER_TEST(cxx_dispatch_mainqueue);
    MU_REGISTER_TEST(cxx_dispatch_cascade);
    MU_REGISTER_TEST(cxx_dispatch_group);
    MU_REGISTER_TEST(cxx_dispatch_serialqueue);
    MU_REGISTER_TEST(cxx_dispatch_semaphore);
    MU_REGISTER_TEST(cxx_dispatch_current);
}

#ifdef __cplusplus
}
#endif

#endif /* CXX_TESTS_H_ */
