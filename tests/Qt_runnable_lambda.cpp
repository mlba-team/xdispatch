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

#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtDispatch/QtDispatch>

#include "Qt_tests.h"

static uintptr_t counter = 0;

class TestRunnable : public QIterationRunnable {
    
public:
    void run(size_t index){
        counter += index;
    }
    
};


static QIterationLambdaRunnable* iter_block_run = NULL;
static QLambdaRunnable* block_run = NULL;

static void exec_runnables() {
    
    for(int i = 0; i < 5; i++)
        iter_block_run->run(i);
    delete iter_block_run;
    MU_ASSERT_EQUAL(counter, 1+2+3+4);
    
    block_run->run();
    delete block_run;
    MU_ASSERT_EQUAL(counter, 1+2+3+4+5);
    
    MU_PASS("");
}


/*
    Testing the various runnables introduced with QtDispatch
 */
extern "C" void Qt_runnable_lambda(){
    MU_BEGIN_TEST(Qt_runnable_lambda);
    
    TestRunnable test_run;
    for(int i = 0; i < 5; i++)
        test_run.run(i);
    MU_ASSERT_EQUAL(counter, 1+2+3+4);


    counter = 0;
    iter_block_run = new QIterationLambdaRunnable([=](size_t index){
        counter += index;
    });
    
    block_run = new QLambdaRunnable([=]{
        counter += 5;
    });
    
    exec_runnables();

    
    MU_END_TEST;
}

