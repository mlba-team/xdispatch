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


#include "../include/xdispatch/dispatch"
#include "cxx_tests.h"

#ifdef XDISPATCH_HAS_BLOCKS

/*
 Little tests mainly checking the correct mapping of the c++ api
 to the underlying C Api
 */

extern "C" void cxx_dispatch_cascade(){
	
    MU_BEGIN_TEST(cxx_dispatch_cascade);

    xdispatch::queue* q = XDISPATCH->global_queue();
	MU_ASSERT_NOT_NULL(q);

	int no = 0;
	
    q->async(${
		int no2 = no+100;
        xdispatch::queue::a_ptr c = XDISPATCH->current_queue();
        c->async(${
			int no3 = no2+20;
            XDISPATCH->current_queue()->async(${
				int no4 = no3+3 ;
                XDISPATCH->current_queue()->async(${
					MU_ASSERT_EQUAL(no4,123);
					MU_PASS("And Out");
                });
			});
		});
    });
	
    XDISPATCH->exec();
	MU_END_TEST;
}

#endif

