/*
* Qt_tests.h
* 
* Copyright (c) 2008-2009 Apple Inc.
* Copyright (c) 2011 MLBA.
* All rights reserved.
* 
* @LICENSE_HEADER_START@
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
* @LICENSE_HEADER_END@
*/

#ifndef QT_TESTS_H_
#define QT_TESTS_H_

#include "munit/MUnit.h"
#include "../libdispatch/platform/platform.h"

#ifdef __cplusplus
# include <QtDispatch/QDispatch>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void Qt_dispatch_timer();
void Qt_free_lambda();
void Qt_dispatch_fibo();
void Qt_dispatch_source_device();
void Qt_dispatch_debug();
void Qt_dispatch_group1();

#if TEST_LAMBDA
void Qt_runnable_lambda();
void Qt_dispatch_cascade_lambda();
void Qt_dispatch_group2_lambda();
void Qt_dispatch_mainqueue_lambda();
void Qt_dispatch_queue_lambda();
void Qt_dispatch_serialqueue_lambda();
void Qt_dispatch_after_lambda();
void Qt_dispatch_semaphore_lambda();
void Qt_dispatch_current_lambda();
void Qt_dispatch_source_signal_lambda();
void Qt_dispatch_source_network_lambda();
void Qt_early_dispatch1_lambda();
void Qt_early_dispatch2_lambda();
#endif

#if TEST_CXX_BLOCKS
void Qt_runnable_blocks();
void Qt_dispatch_cascade_blocks();
void Qt_dispatch_group2_blocks();
void Qt_dispatch_mainqueue_blocks();
void Qt_dispatch_queue_blocks();
void Qt_dispatch_serialqueue_blocks();
void Qt_dispatch_after_blocks();
void Qt_dispatch_semaphore_blocks();
void Qt_dispatch_current_blocks();
void Qt_dispatch_source_signal_blocks();
void Qt_dispatch_source_device_blocks();
void Qt_dispatch_source_network_blocks();
void Qt_free_blocks();
void Qt_early_dispatch1_blocks();
void Qt_early_dispatch2_blocks();
#endif

static void register_qt_tests(){
  MU_REGISTER_TEST(Qt_dispatch_fibo);
  MU_REGISTER_TEST(Qt_dispatch_debug);
  MU_REGISTER_TEST(Qt_dispatch_source_device);
  MU_REGISTER_TEST(Qt_dispatch_timer);
  MU_REGISTER_TEST(Qt_dispatch_group1);

#if TEST_LAMBDA
  MU_REGISTER_TEST(Qt_runnable_lambda);
  MU_REGISTER_TEST(Qt_dispatch_queue_lambda);
  MU_REGISTER_TEST(Qt_dispatch_mainqueue_lambda);
  MU_REGISTER_TEST(Qt_dispatch_cascade_lambda);
  MU_REGISTER_TEST(Qt_dispatch_after_lambda);
  MU_REGISTER_TEST(Qt_dispatch_group2_lambda);
  MU_REGISTER_TEST(Qt_dispatch_serialqueue_lambda);
  MU_REGISTER_TEST(Qt_dispatch_semaphore_lambda);
  MU_REGISTER_TEST(Qt_dispatch_current_lambda);
  MU_REGISTER_TEST(Qt_dispatch_source_signal_lambda);
  MU_REGISTER_TEST(Qt_dispatch_source_network_lambda);
  MU_REGISTER_TEST(Qt_free_lambda);
  MU_REGISTER_TEST(Qt_early_dispatch1_lambda);
  MU_REGISTER_TEST(Qt_early_dispatch2_lambda);
#endif

#if TEST_CXX_BLOCKS
    MU_REGISTER_TEST(Qt_runnable_blocks);
    MU_REGISTER_TEST(Qt_dispatch_queue_blocks);
    MU_REGISTER_TEST(Qt_dispatch_mainqueue_blocks);
    MU_REGISTER_TEST(Qt_dispatch_cascade_blocks);
    MU_REGISTER_TEST(Qt_dispatch_after_blocks);
    MU_REGISTER_TEST(Qt_dispatch_group2_blocks);
    MU_REGISTER_TEST(Qt_dispatch_serialqueue_blocks);
    MU_REGISTER_TEST(Qt_dispatch_semaphore_blocks);
    MU_REGISTER_TEST(Qt_dispatch_current_blocks);
    MU_REGISTER_TEST(Qt_dispatch_source_signal_blocks);
    MU_REGISTER_TEST(Qt_dispatch_source_network_blocks);
    MU_REGISTER_TEST(Qt_free_blocks);
    MU_REGISTER_TEST(Qt_early_dispatch1_blocks);
    MU_REGISTER_TEST(Qt_early_dispatch2_blocks);
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* QT_TESTS_H_ */
