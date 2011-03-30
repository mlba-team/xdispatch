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

#ifndef HAVE_NATIVE_DISPATCH_H

#include <stdlib.h>

#include "../core/internal.h"

#include "tests.h"

/*
  A test for ensuring the core queue implementation works
  */

int counter = 0;

void increment(void* d){
	counter++;
}

void finish(void* d){
	MU_PASS("Queues seem to work");
}

void core_taskqueue()
{
	_taskqueue_t queue = NULL;
	struct _taskitem_s item2 = {NULL, NULL, NULL, 0} , item3 = {NULL, NULL, NULL, 0} ;
	_taskitem_t p_item1, p_item2;
	MU_BEGIN_TEST(core_taskqueue);

	queue = _tq_create();
	MU_ASSERT_NOT_NULL(queue);

	p_item1 = _tq_item();
	MU_ASSERT_NOT_NULL(p_item1);

	p_item1->func = increment;
	item2.func = finish;
	item3.func = increment;

	_tq_push(queue, &item2);
	_tq_push(queue, p_item1);
	_tq_append(queue, &item3);

	MU_ASSERT_NULL(counter);

	p_item2 = _tq_pop(queue);
	MU_ASSERT_EQUAL(p_item2,p_item1);
	p_item2->func(NULL);
	MU_ASSERT_EQUAL(counter,1);

	p_item2 = _tq_last(queue);
	MU_ASSERT_EQUAL(p_item2,&item3);
	_tq_remove(queue,p_item2);
	MU_ASSERT_NOT_EQUAL(&item3, _tq_last(queue));
	p_item2->func(NULL);
	MU_ASSERT_EQUAL(counter,2);

	p_item2 = _tq_first(queue);
	MU_ASSERT_EQUAL(p_item2,&item2);

	//tq_destroy(queue);

	// finish
	p_item2->func(NULL);

	MU_FAIL("Should never reach this");
	MU_END_TEST;
}


#endif
