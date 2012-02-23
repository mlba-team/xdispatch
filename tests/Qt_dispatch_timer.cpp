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

#include <QtDispatch/QtDispatch>
#include <QTime>
#include <QRunnable>

#include "../core/platform/atomic.h"
#include "Qt_tests.h"

static QDispatchTimer* tested_timer = NULL;
static QTime checked_time;

class testPeriodic : public QRunnable {

public:
    testPeriodic() : counter(0) {}

    void run(){

        // we should be executed on the main queue
        MU_ASSERT_EQUAL_HEX(QDispatch::currentQueue().native(), dispatch_get_main_queue());

        // assert that we can really get the timer
        MU_ASSERT_EQUAL_HEX(QDispatchTimer::current(), tested_timer);

        // test the timer interval (but only after the second run
        // as the first one will be started immediately
        if(counter > 0) {
            long diff = checked_time.elapsed();
            MU_DESC_ASSERT_LESS_THAN_EQUAL("timer not too late", diff, 3*1000);
            MU_DESC_ASSERT_LESS_THAN_EQUAL("timer not too early", 1*1000, diff);
        }

        checked_time.restart();

        // only pass when the timer fired at least 5 times
        MU_MESSAGE("\t%i", counter);
        if(counter < 5)
            counter++;
        else {
            MU_PASS("");
        }
    }

    int counter;

};

class testSingleShot : public QRunnable {

public:
    void run(){

        // we should be executed on the global default queue
        MU_ASSERT_EQUAL_HEX(QDispatch::currentQueue().native(), dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0));

        // assert the requested timeout interval
        long diff = checked_time.elapsed();
        MU_DESC_ASSERT_LESS_THAN_EQUAL("single-shot not too late", diff, 3* 1000);
        MU_DESC_ASSERT_LESS_THAN_EQUAL("single-shot not too early", 1*1000, diff);

        // now test wether we can create a periodic timer
        MU_MESSAGE("Testing periodic timer");
        tested_timer = new QDispatchTimer(2000);
        MU_ASSERT_NOT_NULL(tested_timer);
        tested_timer->setHandler(new testPeriodic);
        tested_timer->setTargetQueue(QDispatch::mainQueue());
        checked_time.restart();

        tested_timer->start();
    }

};

extern "C" void Qt_dispatch_timer() {
    char* argv = QString("test").toAscii().data();
    int argc = 1;
    QDispatchApplication app(argc,&argv);

    MU_BEGIN_TEST(Qt_dispatch_timer);

    MU_MESSAGE("Testing single-shot timer");
    checked_time.restart();
    QDispatchTimer::singleShot(dispatch_time(QDispatch::TimeNow, 2*NSEC_PER_SEC), QDispatch::globalQueue(), new testSingleShot());
    //QDispatchTimer::singleShot(QTime::currentTime().addSecs(2), QDispatch::globalQueue(), new test_single_shot);

    app.exec();

    MU_END_TEST
}

#endif /* QT_CORE_LIB */
