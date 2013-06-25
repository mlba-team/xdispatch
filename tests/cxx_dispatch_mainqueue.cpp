/*
* cxx_dispatch_mainqueue.cpp
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

#include <iostream>

#define RUN_TIMES 20

/*
 Little tests mainly checking the correct mapping of the c++ api
 to the underlying C Api
 */

class inc : public xdispatch::iteration_operation {
  public:
    inc(uintptr_t* worker)
      : worker(worker) {

    }

    void operator ()(size_t i){
      dispatch_atomic_inc(worker);
    }

    uintptr_t* worker;
};

class cleanup : public xdispatch::operation {
  public:
    cleanup(uintptr_t* worker)
      : worker(worker) {

    }

    void operator ()(){
      MU_ASSERT_EQUAL(RUN_TIMES, *worker);
      delete worker;
      MU_PASS("");
    }

    uintptr_t* worker;
};

extern "C" void cxx_dispatch_mainqueue(){
    MU_BEGIN_TEST(cxx_dispatch_mainqueue);

	uintptr_t* worker = new uintptr_t;
    *worker = 0;

    xdispatch::queue q = xdispatch::main_queue();
    MU_ASSERT_NOT_NULL(q.native());

    xdispatch::global_queue().apply(new inc(worker), RUN_TIMES);
    q.async(new cleanup(worker));

    xdispatch::exec();
	MU_END_TEST;
}

