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


#include <xdispatch/dispatch>
#include "cxx_tests.h"

#include <iostream>

/*
 Little tests mainly checking the correct mapping of the c++ api
 to the underlying C Api
 */


#define JOBS_NO 20

#if TARGET_OS_EMBEDDED
#define LOOP_COUNT 2000000
#else
#define LOOP_COUNT 100000
#endif

/*
 Little tests mainly checking the correct mapping of the c++ api
 to the underlying C Api
 */

extern "C" void cxx_dispatch_serialqueue_lambda(){
    MU_BEGIN_TEST(cxx_dispatch_serialqueue_lambda);

	unsigned int* worker = new unsigned int;
	*worker = 0;

    xdispatch::queue q("cxx_dispatch_serialqueue");
    MU_ASSERT_NOT_NULL(q.native());

	// dispatch some jobs
	for(unsigned int x = 0; x < JOBS_NO; x++) {
      q.async([=]{
			MU_ASSERT_EQUAL(*worker,x);
			// keep cpu busy
			for(int i = 0; i < LOOP_COUNT;i++);
			*worker = x+1;
        });

	}

    q.sync([=]{
		MU_ASSERT_EQUAL(*worker,JOBS_NO);
		// Test passed
		MU_PASS("Blocks were executed in correct order");
    });

    xdispatch::exec();
	MU_END_TEST;
}

