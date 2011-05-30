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

#include "../include/QtDispatch/QtDispatch"
#include "../core/platform/atomic.h"
#include "Qt_tests.h"

static bool was_freed = false;

struct QtBeFreed2 {

    QtBeFreed2() : ref_ct(new ATOMIC_INT) {
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

    ATOMIC_INT* ref_ct;
};


struct QtBeFreed {

    QtBeFreed() : ref_ct(new ATOMIC_INT) {
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

    ATOMIC_INT* ref_ct;
};

static void Qt_dispatch_outer(){
#ifdef XDISPATCH_HAS_BLOCKS
    QtBeFreed outer;
    QtBeFreed2 inner;

    QDispatch::globalQueue().apply(new QIterationBlockRunnable($(size_t i){
         inner.someFunction();
    }), 10);

    QDispatch::mainQueue().async(new QBlockRunnable(${
        outer.someFunction();
    }));
#endif
}

extern "C" void Qt_free_lambda(){
    MU_BEGIN_TEST(Qt_free_lambda);

    char* argv = QString("test").toAscii().data();
    int argc = 1;
    QDispatchApplication app(argc,&argv);


    Qt_dispatch_outer();
    app.exec();

    MU_END_TEST;
}

#endif /* QT_CORE_LIB */
