/*
* Qt_dispatch_cascade_blocks.cpp
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


#include <QtDispatch/QtDispatch>
#include "Qt_tests.h"

/*
 Little tests mainly checking the correct mapping of the Qt api
 to the underlying C Api
 */

extern "C" void Qt_dispatch_cascade_blocks(){
    QTDISPATCH_decl_argv_argc;
    QDispatchApplication app(argc, argv);
	
    MU_BEGIN_TEST(Qt_dispatch_cascade_blocks);

    QDispatchQueue q = QDispatch::globalQueue(QDispatch::DEFAULT);
    MU_ASSERT_NOT_NULL(q.native());

	int no = 0;
	
    q.async(new QBlockRunnable(^{
		int no2 = no+100;
        QDispatchQueue c = QDispatch::currentQueue();
        c.async(new QBlockRunnable(^{
			int no3 = no2+20;
            QDispatch::currentQueue().async(new QBlockRunnable(^{
				int no4 = no3+3 ;
                QDispatch::currentQueue().async(new QBlockRunnable(^{
					MU_ASSERT_EQUAL(no4,123);
					MU_PASS("And Out");
				}));
			}));
		}));
	}));
	
	app.exec();
	MU_END_TEST;
}
