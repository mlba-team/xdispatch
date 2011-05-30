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

#include <QTime>
#include <QtDispatch/QtDispatch>

#include "Qt_tests.h"

#ifdef XDISPATCH_HAS_BLOCKS

/*
 Little tests mainly checking the correct mapping of the Qt api
 to the underlying C Api
 */

extern "C" void Qt_dispatch_after(){
	QTime watch;
	char* argv = QString("test").toAscii().data();
	int argc = 1;
    QDispatchApplication app(argc,&argv);

        MU_BEGIN_TEST(Qt_dispatch_after);

	watch.start();

    QDispatch::globalQueue(QDispatch::DEFAULT).after(${
		MU_MESSAGE("Should finish between 0.5s and 1.5s: %f", watch.elapsed()/1000.0);
		MU_ASSERT_GREATER_THAN(watch.elapsed(), 700);
	},QTime::currentTime().addMSecs(1000));

    QDispatch::mainQueue().after(${
		MU_MESSAGE("Should finish between 2s and 2.5s: %f", watch.elapsed()/1000.0);
		MU_ASSERT_GREATER_THAN(watch.elapsed(), 2000);
		MU_PASS("");
	},QTime::currentTime().addMSecs(2000));

	app.exec();
	MU_END_TEST;
}

#endif

#endif
