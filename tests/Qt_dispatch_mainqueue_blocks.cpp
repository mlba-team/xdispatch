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


#include <QtCore/QTime>
#include <QtDispatch/QtDispatch>

#include "Qt_tests.h"


#define RUN_TIMES 2000

class MainWorker : public QRunnable {
public:
    MainWorker(unsigned int* work) : worker(work) {}
    void run(){
        if(*worker < RUN_TIMES) {
            (*worker)++;
            //MU_MESSAGE("Running worker %u", *worker);
            QDispatch::mainQueue().async(new MainWorker(worker));
        } else {
            MU_ASSERT_EQUAL(RUN_TIMES, *worker);
            delete worker;
            //MU_MESSAGE("Deleted worker");
            MU_MESSAGE("System usage should be zero");
            QDispatch::mainQueue().after(^{
                MU_PASS("");
            }, QTime::currentTime().addSecs(5));
        }
    }

private:
    unsigned int* worker;
};

/*
 Little tests mainly checking the correct mapping of the Qt api
 to the underlying C Api
 */

extern "C" void Qt_dispatch_mainqueue_blocks(){
	char* argv = QString("test").toAscii().data();
	int argc = 1;
    QDispatchApplication app(argc,&argv);

    MU_BEGIN_TEST(Qt_dispatch_mainqueue_blocks);

    unsigned int* worker = new unsigned int;
    *worker = 0;

    QDispatchQueue q = QDispatch::mainQueue();
    MU_ASSERT_NOT_NULL(q.native());
    q.async(new MainWorker(worker));

	app.exec();
	MU_END_TEST;
}

