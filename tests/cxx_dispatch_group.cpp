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


#include "../include/xdispatch/dispatch"
#include "cxx_tests.h"

#include <iostream>

/*
 Little tests mainly checking the correct mapping of the c++ api
 to the underlying C Api
 */

#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC 1000000000
#endif


class work : public xdispatch::operation {
public:
    work(int d) : delay(d) {}
    void operator()() {
        if (delay) {
            MU_MESSAGE("sleeping...");
            MU_SLEEP(delay);
            MU_MESSAGE("done.");
        }
    }

    int delay;
};

static xdispatch::group* create_group(size_t count, int delay) {
    size_t i;

    xdispatch::group* group = new xdispatch::group();

    for (i = 0; i < count; ++i) {
        xdispatch::queue queue("foo");

        MU_ASSERT_NOT_NULL(queue.native());
        MU_ASSERT_NOT_NULL(group);

        group->async(new work(delay), queue);
    }
    return group;
}


class group_notify : public xdispatch::operation {
public:
    void operator()() {
        xdispatch::queue m = xdispatch::main_queue();
        xdispatch::queue c = xdispatch::current_queue();
        MU_ASSERT_TRUE(m.label() == c.label());
        MU_PASS("Great!");
    }
};

class foo : public xdispatch::operation {
public:
    void operator ()(){

    }
};

void cxx_dispatch_group() {
    bool res;
    xdispatch::group* group;

    MU_BEGIN_TEST(cxx_dispatch_group);

    group = create_group(100, 0);
    MU_ASSERT_NOT_NULL(group);

    group->wait();

    // should be OK to re-use a group
    group->async(new foo, xdispatch::global_queue());
    group->wait();

    delete group;
    group = NULL;

    group = create_group(3, 7);
    MU_ASSERT_NOT_NULL(group);

    res = group->wait(dispatch_time(DISPATCH_TIME_NOW, 5ull * NSEC_PER_SEC));
    MU_ASSERT_EQUAL(res, false);

    // retry after timeout (this time succeed)
    res = group->wait(dispatch_time(DISPATCH_TIME_NOW, 5ull * NSEC_PER_SEC));
    MU_ASSERT_EQUAL(res, true);

    delete group;
    group = NULL;

    group = create_group(100, 0);
    MU_ASSERT_NOT_NULL(group);

    group->notify(new group_notify, xdispatch::main_queue());

    delete group;
    group = NULL;

    xdispatch::exec();

    MU_FAIL("Should never reach this");
    MU_END_TEST
}
