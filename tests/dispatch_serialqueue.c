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

#include <stdlib.h>
#include <xdispatch/dispatch.h>

#include "tests.h"
#include "../core/src/shims/atomic.h"

#define JOBS_NO 20

#if TARGET_OS_EMBEDDED
#define LOOP_COUNT 2000000
#else
#define LOOP_COUNT 100000
#endif

typedef struct {
	int* working;
	int compare;
} context_t;

/*
	A test for ensuring that functions assigned to a serial queue really get
	called in the order they were dispatched.
	*/

static void work(void* data){
	int i = 0;
	context_t* c = (context_t*)data;
	MU_ASSERT_EQUAL(*(c->working),c->compare);
	// keep cpu busy
	for(i = 0; i < LOOP_COUNT;i++);
	*(c->working) = c->compare+1;
	free(data);
}

static void final(void* data){
	context_t* c = (context_t*)data;
	MU_ASSERT_EQUAL(*(c->working),c->compare);
	free(data);
	MU_PASS("Work was done in correct order");
}

void dispatch_apply_serialqueue() {
	dispatch_queue_t  queue;
	int ct = 0;
	int* working = NULL;
	context_t* c = NULL;
	MU_BEGIN_TEST(dispatch_apply_serialqueue);

	queue = dispatch_queue_create("com.mlba.testqueue", 0);
	MU_ASSERT_NOT_NULL(queue);

	working = malloc(sizeof(int));
	MU_ASSERT_NOT_NULL(working);
	*working = 0;

	for(ct=0; ct<JOBS_NO; ct++){
		c = malloc(sizeof(context_t));
		MU_ASSERT_NOT_NULL(c);
		c->working = working;
		c->compare = ct;
		dispatch_async_f(queue,c,work);
	}
	c = malloc(sizeof(context_t));
	MU_ASSERT_NOT_NULL(c);
	c->working = working;
	c->compare = JOBS_NO;
	dispatch_async_f(queue,c,final);

	dispatch_main();

	MU_END_TEST
}

