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

#ifdef WIN32
#	include <windows.h>
#else
#	include <unistd.h>
#	include <time.h>
#endif

#include "../core/shim/atomic.h"
#include "tests.h"

void *ctxt_magic;

static void finalizer(void *ctxt)
{
	MU_ASSERT_EQUAL(ctxt, ctxt_magic);
	MU_PASS("Finalizer run");
}

void test_queue_finalizer() {
	dispatch_queue_t q;
	MU_BEGIN_TEST(test_queue_finalizer);

#ifndef WIN32
	srandom(time(NULL));
#endif
#ifdef __LP64__
	ctxt_magic = (void*)((uintptr_t)rand() << 32 | rand());
#else
	ctxt_magic = (void*)rand();
#endif

	q = dispatch_queue_create(NULL, NULL);
	MU_ASSERT_NOT_NULL(q);

	dispatch_set_context(q, ctxt_magic);

	dispatch_set_finalizer_f(q, finalizer);
	
	dispatch_release(q);

	dispatch_main();
	
	MU_FAIL("Should never reach this");
	MU_END_TEST
}
