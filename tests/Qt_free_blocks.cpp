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


#include <QtDispatch/QtDispatch>

#include "Qt_tests.h"


static bool was_freed = false;

struct QtBeFreed2 {

    QtBeFreed2() : ref_ct(new uintptr_t) {
        *ref_ct = 1;
    }
    QtBeFreed2(const QtBeFreed2& other) : ref_ct(other.ref_ct){
        dispatch_atomic_inc(ref_ct);
    }
    ~QtBeFreed2(){
        if(dispatch_atomic_dec(ref_ct) == 0) {
            delete ref_ct;
            was_freed = true;
        }
    }

    void someFunction() const { /* Do nothing (tm) */ }

    uintptr_t* ref_ct;
};


struct QtBeFreed {

    QtBeFreed() : ref_ct(new uintptr_t) {
        *ref_ct = 1;
    }
    QtBeFreed(const QtBeFreed& other) : ref_ct(other.ref_ct){
        dispatch_atomic_inc(ref_ct);
    }
    ~QtBeFreed(){
        if(dispatch_atomic_dec(ref_ct) == 0) {
            delete ref_ct;
            MU_ASSERT_TRUE(was_freed);
            MU_PASS("");
        }
    }

    void someFunction() const { /* Do nothing (tm) */ }

    uintptr_t* ref_ct;
};

static void Qt_dispatch_outer(){
    QtBeFreed outer;
    QtBeFreed2 inner;

    QDispatch::globalQueue().apply(new QIterationBlockRunnable(^(size_t i){
         inner.someFunction();
    }), 10);

    QDispatch::mainQueue().async(new QBlockRunnable(^{
        outer.someFunction();
    }));
}

extern "C" void Qt_free_blocks(){
    MU_BEGIN_TEST(Qt_free_blocks);

    char* argv = QString("test").toAscii().data();
    int argc = 1;
    QDispatchApplication app(argc,&argv);


    Qt_dispatch_outer();
    app.exec();

    MU_END_TEST;
}

