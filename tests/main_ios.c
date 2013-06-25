/*
* main_ios.c
* 
* Copyright (c) 2011 MLBA.
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

#include "munit/MUnit.h"
#include "tests.h"
#include "cxx_tests.h"
#include "Qt_tests.h"

void null_printer(const char* _unused_){
    
}

/*
 The test library for testing the dispatch
 framework. See also main.c
 */

// call this function from within your
// ios program to run all tests.
// Pass a message handler to print all messages
int run_dispatch_tests(int argc, char* argv[], MU_messageHandler handler) {
	int ret = 0;
    
    if( handler == NULL )
        handler = null_printer;
    
	MU_initFramework( handler );
    
    register_tests();
    register_cxx_tests();
    register_qt_tests();

	ret = MU_main(argc,argv);

	return ret;
}
