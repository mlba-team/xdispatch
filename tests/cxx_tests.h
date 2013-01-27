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

#ifndef CXX_TESTS_H_
#define CXX_TESTS_H_

#include "munit/MUnit.h"
#include "../core/platform/atomic.h"

#ifdef __cplusplus
# include <xdispatch/dispatch>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void cxx_dispatch_debug();
void cxx_dispatch_group();
void cxx_dispatch_mainqueue();
void cxx_dispatch_semaphore();
void cxx_dispatch_current();
void cxx_dispatch_timer();
void cxx_dispatch_once1();
void cxx_synchronized();
void cxx_dispatch_fibo();

#if TEST_BLOCKS
void cxx_dispatch_cascade_blocks();
void cxx_dispatch_group_blocks();
void cxx_dispatch_once2_blocks();
void cxx_dispatch_once3_blocks();
void cxx_dispatch_queue_blocks();
# if DISPATCH_SOURCE_HAS_READ
void cxx_dispatch_read_blocks();
# endif
void cxx_dispatch_serialqueue_blocks();
void cxx_dispatch_source_blocks();
void cxx_free_blocks();
#endif
#if TEST_LAMBDA
void cxx_dispatch_cascade_lambda();
void cxx_dispatch_group_lambda();
void cxx_dispatch_once2_lambda();
void cxx_dispatch_queue_lambda();
# if DISPATCH_SOURCE_HAS_READ
void cxx_dispatch_read_lambda();
# endif
# if DISPATCH_SOURCE_HAS_PROC
void cxx_dispatch_proc_lambda();
# endif
void cxx_dispatch_serialqueue_lambda();
void cxx_dispatch_source_lambda();
void cxx_free_lambda();
#endif

static void register_cxx_tests(){

    MU_REGISTER_TEST(cxx_dispatch_debug);
    MU_REGISTER_TEST(cxx_dispatch_group);
    MU_REGISTER_TEST(cxx_dispatch_mainqueue);
    MU_REGISTER_TEST(cxx_dispatch_semaphore);
    MU_REGISTER_TEST(cxx_dispatch_current);
    MU_REGISTER_TEST(cxx_dispatch_timer);
    MU_REGISTER_TEST(cxx_dispatch_once1);
    MU_REGISTER_TEST(cxx_synchronized);
    MU_REGISTER_TEST(cxx_dispatch_fibo);

#if TEST_BLOCKS
    MU_REGISTER_TEST(cxx_dispatch_cascade_blocks);
    MU_REGISTER_TEST(cxx_dispatch_group_blocks);
    MU_REGISTER_TEST(cxx_dispatch_once2_blocks);
    MU_REGISTER_TEST(cxx_dispatch_once3_blocks);
    MU_REGISTER_TEST(cxx_dispatch_queue_blocks);
# if DISPATCH_SOURCE_HAS_READ
    MU_REGISTER_TEST(cxx_dispatch_read_blocks);
# endif
    MU_REGISTER_TEST(cxx_dispatch_serialqueue_blocks);
    MU_REGISTER_TEST(cxx_dispatch_source_blocks);
    MU_REGISTER_TEST(cxx_free_blocks);
#endif
#if TEST_LAMBDA
    MU_REGISTER_TEST(cxx_dispatch_cascade_lambda);
    MU_REGISTER_TEST(cxx_dispatch_group_lambda);
    MU_REGISTER_TEST(cxx_dispatch_once2_lambda);
    MU_REGISTER_TEST(cxx_dispatch_queue_lambda);
#if DISPATCH_SOURCE_HAS_READ
    MU_REGISTER_TEST(cxx_dispatch_read_lambda);
# endif
# if DISPATCH_SOURCE_HAS_PROC
    MU_REGISTER_TEST(cxx_dispatch_proc_lambda);
# endif
    MU_REGISTER_TEST(cxx_dispatch_serialqueue_lambda);
    MU_REGISTER_TEST(cxx_dispatch_source_lambda);
    MU_REGISTER_TEST(cxx_free_lambda);
#endif

}

#ifdef __cplusplus
}
#endif

#endif /* CXX_TESTS_H_ */
