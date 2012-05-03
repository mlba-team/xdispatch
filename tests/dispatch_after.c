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

#include <stdlib.h>

#include <xdispatch/dispatch.h>

#include "tests.h"

/*
  A test for checking the dispatch_after apis
  */

typedef struct {
	dispatch_time_t min;
	dispatch_time_t max;
	dispatch_time_t start;
} time_delta_t;

static void done(void *arg) {
	//sleep(1);
	MU_PASS("All blocks done");
}

static void dispatch_0(void* context){
	time_delta_t* previous = (time_delta_t*)context;
	dispatch_time_t now_c = dispatch_time(0, 0);
	MU_MESSAGE("must finish between 0.0s and 0.5s: %f",(now_c-previous->start)/(float)NSEC_PER_SEC);
	MU_ASSERT_TRUE(0<=(now_c - previous->min));
	MU_ASSERT_TRUE(0<=(previous->max - now_c));

	dispatch_async_f(dispatch_get_current_queue(), NULL, done);
}

static void dispatch_2(void* context){
	dispatch_time_t time_b;
	time_delta_t* previous = (time_delta_t*)context;
	time_delta_t* delta = (time_delta_t*)malloc(sizeof(time_delta_t));
	dispatch_time_t now_b = dispatch_time(0, 0);
	MU_MESSAGE("must finish between 1.5s and 2.5s: %f",(now_b-previous->start)/(float)NSEC_PER_SEC);
	MU_ASSERT_TRUE(0<=(now_b - previous->min));
	MU_ASSERT_TRUE(0<=(previous->max - now_b));

	delta->min = dispatch_time(0,  0*NSEC_PER_SEC);
	time_b = dispatch_time(0,    0*NSEC_PER_SEC);
	delta->max = dispatch_time(0,  .5*NSEC_PER_SEC);
	delta->start = dispatch_time(0,0);
	dispatch_after_f(time_b, dispatch_get_current_queue(), delta, dispatch_0);
}

static void dispatch_6(void* context){
	dispatch_time_t time_b;
	time_delta_t* previous = (time_delta_t*)context;
	time_delta_t* delta = (time_delta_t*)malloc(sizeof(time_delta_t));
	dispatch_time_t now_a = dispatch_time(0, 0);
	MU_MESSAGE("must finish between 5.5s and 6.5s: %f",(now_a-previous->start)/(float)NSEC_PER_SEC);
	MU_ASSERT_TRUE(0<=(now_a - previous->min));
	MU_ASSERT_TRUE(0<=(previous->max - now_a));

	delta->min = dispatch_time(0,  1.5*NSEC_PER_SEC);
	time_b = dispatch_time(0,    2*NSEC_PER_SEC);
	delta->max = dispatch_time(0,  2.5*NSEC_PER_SEC);
	delta->start = dispatch_time(0,0);
	dispatch_after_f(time_b, dispatch_get_current_queue(), delta, dispatch_2);
}

static void dispatch_start(void* context){
	dispatch_time_t time_a;
	time_delta_t* delta = (time_delta_t*)malloc(sizeof(time_delta_t));
    delta->min = dispatch_time(0,  NSEC_PER_SEC*5.5);
    time_a     = dispatch_time(0,  NSEC_PER_SEC*6.0);
    delta->max = dispatch_time(0,  NSEC_PER_SEC*6.5);
	delta->start = dispatch_time(0,0);
	dispatch_after_f(time_a, dispatch_get_current_queue(), delta, dispatch_6);
}

void dispatch_after_function()
{
	MU_BEGIN_TEST(dispatch_after);

	dispatch_async_f(dispatch_get_main_queue(),NULL,dispatch_start);

	dispatch_main();
	MU_FAIL("Should never reach this");
	MU_END_TEST;
}
