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

#ifdef QT_CORE_LIB

#include <QMutex>
#include <QTime>
#include <xdispatch/QtDispatch/QtDispatch>

#include "Qt_tests.h"

#define ITERATIONS 100000 // 100 000

static bool flag = false;
static size_t counter = 0;
static QDispatchSynclock lock;
static QMutex mutex;

class MutexRun : public QIterationRunnable {

public:
    void run(size_t index){
        Q_UNUSED(index);

        mutex.lock();
        if(flag) {
            counter++;
            flag = false;
        } else
            flag = true;
        mutex.unlock();

    }

};

class SynchronizedRun : public QIterationRunnable {

public:
    void run(size_t index){
        Q_UNUSED(index);

        synchronized {
            if(flag) {
                counter++;
                flag = false;
            } else
                flag = true;
        }

    }

};

class PartialRun1 : public QIterationRunnable {

public:
    void run(size_t index){
        Q_UNUSED(index);

        synchronize(lock) {
            if(flag) {
                counter++;
                flag = false;
            } else
                flag = true;
        }
    }

};

class PartialRun2 : public QIterationRunnable {

public:
    void run(size_t index){
        Q_UNUSED(index);

        synchronize(lock){
            if(flag) {
                counter++;
                flag = false;
            } else
                flag = true;
        }
    }

};

/**
  Test the 'synchronized' statements
  */
extern "C" void Qt_synchronized() {
    QTime watch;
    QDispatchQueue* q = QD->getGlobalQueue();

    MU_BEGIN_TEST(Qt_synchronized);

    // test 1 1/2
    MU_MESSAGE("Testing 'synchronized' keyword (apply)");
    QIterationRunnable* test1 = new SynchronizedRun;
    test1->setAutoDelete(true);
    watch.restart();
    QD->getGlobalQueue()->dispatch(test1, ITERATIONS);
    MU_ASSERT_EQUAL(counter, ITERATIONS/2);
    MU_MESSAGE("%f ms per Iteration", watch.elapsed() / (double)ITERATIONS);

    // reset
    flag = false;
    counter = 0;

    // test 1 2/2
    MU_MESSAGE("Testing 'synchronized' keyword (for / group)");
    test1 = new SynchronizedRun;
    test1->setAutoDelete(false);
    QDispatchGroup group1;
    q->suspend();
    for(int i = 0; i < ITERATIONS; i++){
        group1.dispatch(test1, q);
        group1.dispatch(test1, q);
    }
    watch.restart();
    q->resume();
    group1.wait();
    MU_ASSERT_EQUAL(counter, ITERATIONS);
    MU_MESSAGE("%f ms per Iteration", watch.elapsed() / (double)ITERATIONS);

    // reset
    flag = false;
    counter = 0;

    // test 2 1/2
    MU_MESSAGE("Testing 'synchronize' keyword (apply)");
    QIterationRunnable* test2a = new PartialRun1;
    test2a->setAutoDelete(true);
    watch.restart();
    QD->getGlobalQueue()->dispatch(test2a, ITERATIONS);
    MU_ASSERT_EQUAL(counter, ITERATIONS/2);
    MU_MESSAGE("%f ms per Iteration", watch.elapsed() / (double)ITERATIONS);

    // reset
    flag = false;
    counter = 0;

    // test 2 2/2
    MU_MESSAGE("Testing 'synchronize' keyword (for / group)");
    test2a = new PartialRun1;
    test2a->setAutoDelete(false);
    QIterationRunnable* test2b = new PartialRun2;
    test2b->setAutoDelete(false);
    QDispatchGroup group2;
    q->suspend();
    for(int i = 0; i < ITERATIONS; i++){
        group2.dispatch(test2a, q);
        group2.dispatch(test2b, q);
    }
    watch.restart();
    q->resume();
    group2.wait();
    MU_ASSERT_EQUAL(counter, ITERATIONS);
    MU_MESSAGE("%f ms per Iteration", watch.elapsed() / (double)ITERATIONS);

    // reset
    flag = false;
    counter = 0;

    // test 3
    MU_MESSAGE("Testing with normal mutex (apply)");
    QIterationRunnable* mutexRun = new MutexRun;
    mutexRun->setAutoDelete(true);
    watch.restart();
    QD->getGlobalQueue()->dispatch(mutexRun, ITERATIONS);
    MU_ASSERT_EQUAL(counter, ITERATIONS/2);
    MU_MESSAGE("%f ms per Iteration", watch.elapsed() / (double)ITERATIONS);

    // cleanup
    delete test2a;
    delete test2b;
    delete test1;

    MU_PASS("Yay");

    MU_END_TEST
}

#endif
