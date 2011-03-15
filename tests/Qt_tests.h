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

#ifndef QT_TESTS_H_
#define QT_TESTS_H_

#include "munit/MUnit.h"
#include "../bc/blocks.h"


/* detect wether to build blocks tests or not (the autodetection only works
   safely when building in cpp mode) */
#ifndef HAS_BLOCKS
#	if (defined HAVE_BLOCKS_IMPL || defined __BLOCKS__ || _MSC_VER >= 1600)
#		define TEST_BLOCKS
#	endif
#else
#	define TEST_BLOCKS
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef QT_CORE_LIB
void Qt_dispatch_debug();
void Qt_dispatch_cascade();
void Qt_dispatch_group();
void Qt_dispatch_mainqueue();
void Qt_dispatch_queue();
void Qt_dispatch_serialqueue();
void Qt_dispatch_after();
void Qt_dispatch_semaphore();
#endif

static void register_qt_tests(){
#ifdef QT_CORE_LIB
#ifdef TEST_BLOCKS
	MU_REGISTER_TEST(Qt_dispatch_debug);
	MU_REGISTER_TEST(Qt_dispatch_queue);
	MU_REGISTER_TEST(Qt_dispatch_mainqueue);
	MU_REGISTER_TEST(Qt_dispatch_cascade);
	MU_REGISTER_TEST(Qt_dispatch_after);
	MU_REGISTER_TEST(Qt_dispatch_group);
	MU_REGISTER_TEST(Qt_dispatch_serialqueue);
	MU_REGISTER_TEST(Qt_dispatch_semaphore);
#endif
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* QT_TESTS_H_ */
