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

#include <QCoreApplication>

#include "../xdispatch/QtDispatch/QtDispatch"
#include "Qt_tests.h"

#ifdef HAS_BLOCKS

/*
 Little tests mainly checking the correct mapping of the Qt api
 to the underlying C Api
 */

extern "C" void Qt_dispatch_cascade(){
	char* argv = QString("test").toAscii().data();
	int argc = 1;
	QCoreApplication app(argc,&argv);
	
	MU_BEGIN_TEST(Qt_dispatch_cascade);

	QDispatchQueue* q = QDispatch::instance->getGlobalQueue(QDispatch::DEFAULT);
	MU_ASSERT_NOT_NULL(q);

	int no = 0;
	
	q->dispatch(new QBlockRunnable(${
		int no2 = no+100;
        QDispatchQueue::APtr c = QDispatch::instance->getCurrentQueue();
		c->dispatch(${
			int no3 = no2+20;
			QDispatch::instance->getCurrentQueue()->dispatch(${
				int no4 = no3+3 ;
				QDispatch::instance->getCurrentQueue()->dispatch(new QBlockRunnable(${
					MU_ASSERT_EQUAL(no4,123);
					MU_PASS("And Out");
				}));
			});
		});
	}));
	
	app.exec();
	MU_END_TEST;
}

#endif
#endif