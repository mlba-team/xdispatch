/*
* cxx_free_blocks.cpp
* 
* Copyright (c) 2008-2009 Apple Inc.
* Copyright (c) 2011-2013 MLBA-Team.
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


#include <xdispatch/dispatch>
#include "cxx_tests.h"

static bool was_freed = false;

struct BeFreed2 {

    BeFreed2() : ref_ct(new uintptr_t) {
        *ref_ct = 1;
    }
    BeFreed2(const BeFreed2& other) : ref_ct(other.ref_ct){
        dispatch_atomic_inc(ref_ct);
    }
    ~BeFreed2(){
        if(dispatch_atomic_dec(ref_ct) == 0) {
            delete ref_ct;
            was_freed = true;
        }
    }

    void someFunction() const { /* Do nothing (tm) */ }

    uintptr_t* ref_ct;
};


struct BeFreed {

    BeFreed() : ref_ct(new uintptr_t) {
        *ref_ct = 1;
    }
    BeFreed(const BeFreed& other) : ref_ct(other.ref_ct){
        dispatch_atomic_inc(ref_ct);
    }
    ~BeFreed(){
        if(dispatch_atomic_dec(ref_ct) == 0) {
            delete ref_ct;
            MU_ASSERT_TRUE(was_freed);
            MU_PASS("");
        }
    }

    void someFunction() const { /* Do nothing (tm) */ }

    uintptr_t* ref_ct;
};

static void dispatch_outer(){
    BeFreed outer;
    BeFreed2 inner;

    xdispatch::global_queue().apply(^(size_t i){
         inner.someFunction();
    }, 10);

    xdispatch::main_queue().async(^{
        outer.someFunction();
    });
}

extern "C" void cxx_free_blocks(){
    MU_BEGIN_TEST(cxx_free_blocks);

    dispatch_outer();
    xdispatch::exec();

    MU_END_TEST;
}
