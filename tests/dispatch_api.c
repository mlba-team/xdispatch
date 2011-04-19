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

#include "tests.h"

#include <xdispatch/dispatch.h>

static pthread_t main_thread;

/*
 A simple test for ensuring that getting the global
 queues is actually working
 */

void pass(void* dt){
    pthread_t now = pthread_self();
    MU_ASSERT_EQUAL(pthread_equal(now, main_thread), 1);
    MU_PASS("Core API is working");
}

void dispatch_api() {
    dispatch_queue_t q = NULL;

	MU_BEGIN_TEST(dispatch_api);

    main_thread = pthread_self();
    q = dispatch_get_main_queue();
	MU_ASSERT_NOT_NULL(q);
    dispatch_async_f(q, NULL, pass);

	q = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,0);
	MU_ASSERT_NOT_NULL(q);
	q = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW,0);
	MU_ASSERT_NOT_NULL(q);
	q = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH,0);
	MU_ASSERT_NOT_NULL(q);

    dispatch_main();

	MU_END_TEST;
}
