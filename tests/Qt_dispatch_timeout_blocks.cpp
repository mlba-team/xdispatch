/*
* Qt_dispatch_timeout_blocks.cpp
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

#include <QtCore/QTime>
#include <QtDispatch/QtDispatch>

#include "Qt_tests.h"

/*
 Little tests mainly checking the correct mapping of the Qt api
 to the underlying C Api
 */

extern "C" void Qt_dispatch_after_blocks(){
	QTime watch;
    QTDISPATCH_decl_argv_argc;
    QDispatchApplication app(argc, argv);
        MU_BEGIN_TEST(Qt_dispatch_after_blocks);

	watch.start();

    QDispatch::globalQueue(QDispatch::DEFAULT).after(^{
		MU_MESSAGE("Should finish between 0.5s and 1.5s: %f", watch.elapsed()/1000.0);
		MU_ASSERT_GREATER_THAN_EQUAL(watch.elapsed(), 700);
	},QTime::currentTime().addMSecs(1000));

    QDispatch::mainQueue().after(^{
		MU_MESSAGE("Should finish between 2s and 2.5s: %f", watch.elapsed()/1000.0);
		MU_ASSERT_GREATER_THAN_EQUAL(watch.elapsed(), 1800);
		MU_PASS("");
	},QTime::currentTime().addMSecs(2000));

	app.exec();
	MU_END_TEST;
}
