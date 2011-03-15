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

#ifdef QT_CORE_LIB

#include <QDebug>
#include "../QtDispatch"

#include "Qt_tests.h"

/*
 Little tests mainly checking the correct mapping of the Qt api
 to the underlying C Api
 */

extern "C" void Qt_dispatch_debug(){
	

    MU_BEGIN_TEST(Qt_dispatch_debug);

	QDispatchQueue* q = QD->getMainQueue();
	QDispatchGroup g;
	QDispatchSemaphore s(0);
	
	MU_MESSAGE("Begin testing debug output using QDebug()");

	qDebug() << q;
	qDebug() << g;
	qDebug() << s;

	delete q;

	MU_MESSAGE("All output done.");

	MU_END_TEST;
}

#endif
