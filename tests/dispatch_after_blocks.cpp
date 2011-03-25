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

#include <xdispatch/dispatch.h>

#ifdef XDISPATCH_HAS_BLOCKS

#include "tests.h"

/*
	A test checking the dispatch_after apis by using blocks
	*/

void done(void *arg) {
	//sleep(1);
	MU_PASS("All blocks done");
}

extern "C" void
		dispatch_after_blocks(void)
{
	MU_BEGIN_TEST(dispatch_after_blocks);

	dispatch_async(dispatch_get_main_queue(), ${
		dispatch_time_t time_a_min = dispatch_time(0,  5.5*NSEC_PER_SEC);
		dispatch_time_t time_a     = dispatch_time(0,   6*NSEC_PER_SEC);
		dispatch_time_t time_a_max = dispatch_time(0,  6.5*NSEC_PER_SEC);
		dispatch_time_t time_a_start = dispatch_time(0,0);
		dispatch_after(time_a, dispatch_get_current_queue(), ${
			dispatch_time_t now_a = dispatch_time(0, 0);
			MU_MESSAGE("must finish between 5.5s and 6.5s: %f",(now_a-time_a_start)/(float)NSEC_PER_SEC);
			MU_ASSERT_TRUE(0<=(now_a - time_a_min));
			MU_ASSERT_TRUE(0<=(time_a_max - now_a));

			dispatch_time_t time_b_min = dispatch_time(0,  1.5*NSEC_PER_SEC);
			dispatch_time_t time_b     = dispatch_time(0,    2*NSEC_PER_SEC);
			dispatch_time_t time_b_max = dispatch_time(0,  2.5*NSEC_PER_SEC);
			dispatch_time_t time_b_start = dispatch_time(0,0);
			dispatch_after(time_b, dispatch_get_current_queue(), ${
				dispatch_time_t now_b = dispatch_time(0, 0);
				MU_MESSAGE("must finish between 1.5s and 2.5s: %f",(now_b-time_b_start)/(float)NSEC_PER_SEC);
				MU_ASSERT_TRUE(0<=(now_b - time_b_min));
				MU_ASSERT_TRUE(0<=(time_b_max - now_b));

				dispatch_time_t time_c_min = dispatch_time(0,  0*NSEC_PER_SEC);
				dispatch_time_t time_c     = dispatch_time(0,  0*NSEC_PER_SEC);
				dispatch_time_t time_c_max = dispatch_time(0,  .5*NSEC_PER_SEC);
				dispatch_time_t time_c_start = dispatch_time(0, 0);
				dispatch_after(time_c, dispatch_get_current_queue(), ${
					dispatch_time_t now_c = dispatch_time(0, 0);
					MU_MESSAGE("must finish between 0s and .5s:  %f",(now_c-time_c_start)/(float)NSEC_PER_SEC);
					MU_ASSERT_TRUE(0<=(now_c - time_c_min));
					MU_ASSERT_TRUE(0<=(time_c_max - now_c));

					dispatch_async_f(dispatch_get_current_queue(), NULL, done);
				});
			});
		});
	});

	dispatch_main();
	MU_FAIL("Should never reach this");
	MU_END_TEST;
}

#endif
