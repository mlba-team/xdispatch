/*
* main.c
* 
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

#include "munit/MUnit.h"
#if DISPATCH_C_SUPPORT
# include "tests.h"
#else
  static void register_tests(){}
#endif
#include "cxx_tests.h"

#if XDISPATCH_QT_SUPPORT
# include "Qt_tests.h"
#else
  static void register_qt_tests(){}
#endif

void print_log(const char* msg) {
    printf("%s", msg);
    fflush(stdout);
}

/*
 The test program for testing the dispatch
 framework. The difficult task in here is
 that all functions dispatched can be started
 at any time. Thus each test has to run as own
 process, end when all functions were executed
 an be supervised by the test application
 */

int main(int argc, char* argv[]) {
	int ret = 0;
    MU_initFramework( print_log );
    register_tests();
    register_cxx_tests();
    register_qt_tests();

	ret = MU_main(argc,argv);

	return ret;
}
