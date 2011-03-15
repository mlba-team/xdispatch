/*
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

#include "tests.h"
#include "Qt_tests.h"

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
	MU_initFramework();
	register_tests();
	register_qt_tests();

	ret = MU_main(argc,argv);

	return ret;
}
