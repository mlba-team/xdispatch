/*
* dispatch_debug.c
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

#include <stdio.h>
#include <xdispatch/dispatch.h>
#include <stdlib.h>
#include <string.h>

#include "tests.h"


void dispatch_testDebug() {
	dispatch_queue_t default_q, main_q;
	dispatch_group_t g;
	MU_BEGIN_TEST(dispatch_testDebug);
	MU_MESSAGE("Printing object debugging information:");

	main_q = dispatch_get_main_queue();
    MU_ASSERT_NOT_NULL(main_q);
	dispatch_debug(main_q, "dispatch_queue_t");
	
	default_q = dispatch_get_global_queue(0, 0);
    MU_ASSERT_NOT_NULL(default_q);
	dispatch_debug(default_q, "dispatch_queue_t");

	g = dispatch_group_create();
    MU_ASSERT_NOT_NULL(g);
	dispatch_debug(g, "dispatch_group_t");

	MU_MESSAGE("Printing done.");
	MU_PASS("");
	MU_END_TEST
}
