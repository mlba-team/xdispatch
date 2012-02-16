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
#include "../core/platform/platform.h"

#ifdef __cplusplus
# include <QtDispatch/QDispatch>
#endif


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

#if XDISPATCH_QT_SUPPORT
void Qt_runnable();
void Qt_dispatch_debug();
void Qt_dispatch_cascade();
void Qt_dispatch_group1();
void Qt_dispatch_group2();
void Qt_dispatch_mainqueue();
void Qt_dispatch_queue();
void Qt_dispatch_serialqueue();
void Qt_dispatch_after();
void Qt_dispatch_semaphore();
void Qt_dispatch_current();
void Qt_synchronized();
void Qt_dispatch_timer();
void Qt_dispatch_source_signal();
void Qt_dispatch_source_device();
void Qt_dispatch_source_network();
void Qt_free_lambda();
void Qt_early_dispatch1();
void Qt_early_dispatch2();
void Qt_dispatch_fibo();
#endif

static void register_qt_tests(){
#ifdef XDISPATCH_QT_SUPPORT
#ifdef TEST_BLOCKS
    MU_REGISTER_TEST(Qt_runnable);
	MU_REGISTER_TEST(Qt_dispatch_debug);
	MU_REGISTER_TEST(Qt_dispatch_queue);
	MU_REGISTER_TEST(Qt_dispatch_mainqueue);
	MU_REGISTER_TEST(Qt_dispatch_cascade);
	MU_REGISTER_TEST(Qt_dispatch_after);
    MU_REGISTER_TEST(Qt_dispatch_group1);
    MU_REGISTER_TEST(Qt_dispatch_group2);
	MU_REGISTER_TEST(Qt_dispatch_serialqueue);
	MU_REGISTER_TEST(Qt_dispatch_semaphore);
    MU_REGISTER_TEST(Qt_dispatch_current);
    MU_REGISTER_TEST(Qt_dispatch_timer);
    MU_REGISTER_TEST(Qt_synchronized);
	MU_REGISTER_TEST(Qt_dispatch_source_signal);
	MU_REGISTER_TEST(Qt_dispatch_source_device);
	MU_REGISTER_TEST(Qt_dispatch_source_network);
    MU_REGISTER_TEST(Qt_free_lambda);
    MU_REGISTER_TEST(Qt_early_dispatch1);
    MU_REGISTER_TEST(Qt_early_dispatch2);
    MU_REGISTER_TEST(Qt_dispatch_fibo);
#endif
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* QT_TESTS_H_ */
