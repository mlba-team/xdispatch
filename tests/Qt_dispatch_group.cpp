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

#include <QTest>
#include <QObject>
#include <QCoreApplication>
#include <QtDispatch/QtDispatch>

#include "Qt_tests.h"
#include "../core/atomic.h"

static unsigned int* worker = 0;

class GroupTest : public QObject {

    Q_OBJECT

public slots:
    void notify(){
        MU_MESSAGE("signal 'all_finished' emitted");

        if(*worker == 0)
            return;

        MU_ASSERT_EQUAL(*worker,4);
        delete worker;
        this->deleteLater();
        MU_PASS("All Done");
    }

};

#ifdef XDISPATCH_HAS_BLOCKS

/*
 Little tests mainly checking the correct mapping of the Qt api
 to the underlying C Api
 */

extern "C" void Qt_dispatch_group(){
	char* argv = QString("test").toAscii().data();
	int argc = 1;
	QCoreApplication app(argc,&argv);

	MU_BEGIN_TEST(Qt_dispatch_group);

    worker = new unsigned int;
	*worker = 0;

	QDispatchGroup group;
    GroupTest* gt = new GroupTest;
    QObject::connect(&group, SIGNAL(all_finished()), gt, SLOT(notify()) );

    group.async(${
		QTest::qSleep(400);
		atomic_inc_get(worker);
	});

    group.async(${
		QTest::qSleep(300);
		atomic_inc_get(worker);
	});

    group.async(${
		QTest::qSleep(300);
		atomic_inc_get(worker);
	});

    group.async(${
		QTest::qSleep(200);
		atomic_inc_get(worker);
	});

	app.exec();
	MU_END_TEST;
}

#endif

#include <moc_Qt_dispatch_group.moc>

#endif
