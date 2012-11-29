/*
* Copyright (c) 2008-2009 Apple Inc. All rights reserved.
* Copyright (c) 2011-2012 MLBA-Team. All rights reserved.
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


#include <xdispatch/dispatch>
#include "cxx_tests.h"

#include <iostream>

/*
 Little tests mainly checking the correct mapping of the c++ api
 to the underlying C Api
 */

extern "C" void cxx_dispatch_debug(){
    MU_BEGIN_TEST(cxx_dispatch_debug);

    xdispatch::queue q = xdispatch::main_queue();
    xdispatch::group g;
    xdispatch::semaphore s(0);
	
    MU_MESSAGE("Begin testing debug output using std::cout");

    std::cout << q << std::endl;
    std::cout << g << std::endl;
    std::cout << s << std::endl;

	MU_MESSAGE("All output done.");

	MU_END_TEST;
}
