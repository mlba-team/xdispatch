/*
* Qt_dispatch_group_blocks.cpp
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

#include <QTest>
#include <QtCore/QObject>
#include <QtCore/QCoreApplication>
#include <QtDispatch/QtDispatch>

#include "Qt_tests.h"


// // Test the signal implementation

static uintptr_t* worker = 0;

class GroupTestBlocks : public QObject {

        Q_OBJECT

    public slots:
        void notify(){
            MU_MESSAGE("signal 'allFinished' emitted, worker = %u", *worker);

            if(*worker == 0)
                return;

            MU_ASSERT_EQUAL(*worker,4);
            delete worker;
            this->deleteLater();
            MU_PASS("All Done");
        }

};

extern "C" void Qt_dispatch_group2_blocks(){
    char argv[] = "test";
    int argc = 1;
    QDispatchCoreApplication app(argc, (char**)&argv);

    MU_BEGIN_TEST(Qt_dispatch_group2_blocks);

    worker = new uintptr_t;
    *worker = 0;

    QDispatchGroup group;
    GroupTestBlocks* gt = new GroupTestBlocks;
    QObject::connect(&group, SIGNAL(allFinished()), gt, SLOT(notify()) );

    group.async(^{
        QTest::qSleep(400);
        dispatch_atomic_inc(worker);
    });

    group.enableAllFinishedSignal();

    group.async(^{
        QTest::qSleep(300);
        dispatch_atomic_inc(worker);
    });

    group.async(^{
       QTest::qSleep(300);
        dispatch_atomic_inc(worker);
    });

    group.async(^{
        QTest::qSleep(200);
        dispatch_atomic_inc(worker);
    });

    app.exec();
    MU_END_TEST;
}

#include <moc_Qt_dispatch_group_blocks.moc>
