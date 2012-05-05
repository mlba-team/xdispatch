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


#include <QtCore/QTime>
#include <QtDispatch/QtDispatch>

#include "Qt_tests.h"


#ifdef XDISPATCH_HAS_BLOCKS

/*
 Verify that jobs dispatched before creation
 of the QDispatchApplication are properly processed
 */

extern "C" void Qt_early_dispatch1(){
    MU_BEGIN_TEST(Qt_early_dispatch1);

    QDispatchQueue q = QDispatch::mainQueue();
    MU_ASSERT_NOT_NULL( q.native() );
    q.async( ${ MU_PASS(""); } );

	char* argv = QString("test").toAscii().data();
	int argc = 1;
    QDispatchApplication app(argc,&argv);

	app.exec();
	MU_END_TEST;
}

extern "C" void Qt_early_dispatch2(){
    MU_BEGIN_TEST(Qt_early_dispatch2);

    QDispatchQueue q = QDispatch::mainQueue();
    MU_ASSERT_NOT_NULL( q.native() );
    q.async( ${ MU_PASS(""); } );

    char* argv = QString("test").toAscii().data();
    int argc = 1;
    QDispatchCoreApplication app(argc,&argv);

    app.exec();
    MU_END_TEST;
}

#endif

#endif
