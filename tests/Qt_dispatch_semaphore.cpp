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

#ifdef QT_CORE_LIB

#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtDispatch/QtDispatch>

#include "Qt_tests.h"

#ifdef XDISPATCH_HAS_BLOCKS

#define LAPS 10000

/*
Little tests mainly checking the correct mapping of the Qt api
to the underlying C Api
*/

extern "C" void Qt_dispatch_semaphore(){
	static size_t total;

	MU_BEGIN_TEST(Qt_dispatch_semaphore);
	QDispatchSemaphore* dsema = new QDispatchSemaphore(1);

    QDispatchQueue("Qt_dispatch_semaphore").apply($(size_t idx) {
        dsema->try_acquire(DISPATCH_TIME_FOREVER);
		total++;
		dsema->release();
	}, LAPS);

	delete dsema;

	MU_ASSERT_EQUAL(total, LAPS);

	MU_PASS("");
	MU_END_TEST;
}

#endif

#endif
