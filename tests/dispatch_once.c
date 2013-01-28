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

#include "tests.h"

#include <xdispatch/dispatch.h>

/*
 A (very) simple test for ensuring the once
 methods are properly working.
 NOTE: Still lacking a parallel test
 */

static dispatch_once_t once_obj;
static int once_counter = 0;

static void increment(void* unused) {
    once_counter++;
}

void test_dispatch_once() {
    MU_BEGIN_TEST(test_dispatch_once);

    MU_ASSERT_EQUAL( once_counter, 0 );
    dispatch_once_f( &once_obj, NULL, increment );
    dispatch_once_f( &once_obj, NULL, increment );
    dispatch_once_f( &once_obj, NULL, increment );
    dispatch_once_f( &once_obj, NULL, increment );
    dispatch_once_f( &once_obj, NULL, increment );
    MU_ASSERT_EQUAL( once_counter, 1 );

    MU_PASS("");

	MU_END_TEST;
}
