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

#include <stdio.h>
#include <xdispatch/dispatch.h>
#include <stdlib.h>
#include <string.h>

#include "../core/platform/atomic.h"
#include "tests.h"

static xdispatch::timer* tested_timer = NULL;
static struct timeval checked_time;

class test_periodic : public xdispatch::operation {

public:
    test_periodic() : counter(0) {

    }

    void operator ()(){

        // we should be executed on the main queue
        MU_ASSERT_EQUAL(xdispatch::current_queue().native(), dispatch_get_main_queue());

        // assert that we can really get the timer
        MU_ASSERT_EQUAL(xdispatch::timer::current(), tested_timer);

        // test the timer interval (but only after the second run
        // as the first one will be started immediately
        if(counter > 0) {
            struct timeval now;
            gettimeofday(&now, NULL);
            long diff = (now.tv_sec - checked_time.tv_sec);
            MU_DESC_ASSERT_LESS_THAN_DOUBLE("timer not too late", diff, 3.0);
            MU_DESC_ASSERT_LESS_THAN_DOUBLE("timer not too early", 1.0, diff);
        }

        gettimeofday(&checked_time, NULL);

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

class test_single_shot : public xdispatch::operation {

public:
    void operator ()(){

        // we should be executed on the global default queue
        MU_ASSERT_EQUAL(xdispatch::current_queue().native(), dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0));

        // assert the requested timeout interval
        struct timeval end_time;
        gettimeofday(&end_time, NULL);

        long diff = end_time.tv_sec - checked_time.tv_sec;
        MU_DESC_ASSERT_LESS_THAN("single-shot not too late", diff, 4);
        MU_DESC_ASSERT_LESS_THAN("single-shot not too early", 1, diff);

        // now test wether we can create a periodic timer
        MU_MESSAGE("Testing periodic timer");
        tested_timer = new xdispatch::timer(2*xdispatch::nsec_per_sec);
        MU_ASSERT_NOT_NULL(tested_timer);
        MU_ASSERT_NOT_NULL(tested_timer->native());
        tested_timer->handler(new test_periodic);
        tested_timer->target_queue(xdispatch::main_queue());
        gettimeofday(&checked_time, NULL);

        tested_timer->start();
    }

};

extern "C" void cxx_dispatch_timer() {
    MU_BEGIN_TEST(cxx_dispatch_timer);

    MU_MESSAGE("Testing single-shot timer");
    gettimeofday(&checked_time, NULL);
    xdispatch::timer::single_shot(dispatch_time(xdispatch::time_now, xdispatch::nsec_per_sec*2), xdispatch::global_queue(), new test_single_shot);
    xdispatch::exec();

    MU_END_TEST
}
