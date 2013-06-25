/*
* cxx_dispatch_current.cpp
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

/*
 Little tests mainly checking the correct mapping of the c++ api
 to the underlying C Api
 */

class main_queue : public xdispatch::operation {
public:
    void operator()() {
        MU_MESSAGE("Queue should be main queue");
        std::cout << xdispatch::current_queue() << std::endl;

        MU_PASS("");
    }
};

class global_queue : public xdispatch::operation {
public:
    void operator() (){
        MU_MESSAGE("Queue should be global default queue");
        std::cout << xdispatch::current_queue() << std::endl;

        xdispatch::main_queue().async(new main_queue);
    }
};

extern "C" void cxx_dispatch_current(){
    MU_BEGIN_TEST(cxx_dispatch_current);
    
    xdispatch::global_queue().async(new global_queue);

    xdispatch::exec();
    MU_END_TEST;
}
