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

#include <stdio.h>
#include <xdispatch/dispatch.h>
#include <stdlib.h>
#include <string.h>

#include "../core/platform/atomic.h"
#include "tests.h"

#ifdef TEST_BLOCKS

//
// There were several bugs related to sign extension / integer overflow that
// were encountered during development. Create a timer whose interval has the
// 31st bit of the word set to verify that it behaves correctly.
//
// 2 s < 0x80000000ull ns < 4 s
//

extern "C" void dispatch_timer_bit31() {
    MU_BEGIN_TEST(dispatch_timer_bit31);

	MU_PASS("");

    dispatch_queue_t main_q = dispatch_get_main_queue();
    MU_ASSERT_EQUAL(main_q, dispatch_get_current_queue());

	dispatch_source_t timer;
	struct timeval start_time;
	
	gettimeofday(&start_time, NULL);

	timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, main_q);
    MU_ASSERT_NOT_NULL(timer);

	dispatch_source_set_timer(timer, dispatch_time(DISPATCH_TIME_NOW, 0x80000000ull), 0, 0);

    dispatch_source_set_event_handler(timer, ${
		dispatch_source_cancel(timer);
	});

    dispatch_source_set_cancel_handler(timer, ${
		struct timeval end_time;
		gettimeofday(&end_time, NULL);

        MU_ASSERT_LESS_THAN(end_time.tv_sec - start_time.tv_sec, 4);
        MU_ASSERT_LESS_THAN(1, end_time.tv_sec - start_time.tv_sec);
        MU_PASS("");
	});
	
	dispatch_resume(timer);
	
    dispatch_main();

    MU_END_TEST
}

#endif /* TEST_BLOCKS */
