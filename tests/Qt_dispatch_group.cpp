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
#include "../core/platform/atomic.h"



// // Test the 'normal' API mapping

#ifdef XDISPATCH_HAS_BLOCKS

class Work : public QRunnable {
    public:
        Work(int d) : delay(d) {}
        ~Work(){}
        void run() {
            if (delay) {
                MU_MESSAGE("sleeping...");
                MU_SLEEP(delay);
                MU_MESSAGE("done.");
            }
        }

        int delay;
};

static QDispatchGroup* createGroup(size_t count, int delay) {
    size_t i;

    QDispatchGroup* group = new QDispatchGroup();

    for (i = 0; i < count; ++i) {
        QDispatchQueue queue("foo");

        MU_ASSERT_NOT_NULL(queue.native());
        MU_ASSERT_NOT_NULL(group);

        group->async(new Work(delay), queue);
    }
    return group;
}


class GroupNotify : public QRunnable {

    public:
        GroupNotify(){}
        ~GroupNotify(){}

        void run() {
            QDispatchQueue m = QDispatch::mainQueue();
            QDispatchQueue c = QDispatch::currentQueue();
            MU_ASSERT_TRUE(m.label() == c.label());
            MU_PASS("Great!");
        }
};

class Foo : public QRunnable {
    public:
        void run(){

        }
};

extern "C" void Qt_dispatch_group1() {
    bool res;
    QDispatchGroup* group;
    char* argv = QString("test").toAscii().data();
    int argc = 1;
    QDispatchCoreApplication app(argc,&argv);

    MU_BEGIN_TEST(Qt_dispatch_group1);

    group = createGroup(100, 0);
    MU_ASSERT_NOT_NULL(group);

    group->wait();

    // should be OK to re-use a group
    group->async(new Foo, QDispatch::globalQueue());
    group->wait();

    delete group;
    group = NULL;

    group = createGroup(3, 7);
    MU_ASSERT_NOT_NULL(group);

    res = group->wait(dispatch_time(QDispatch::TimeNow, 5ull * NSEC_PER_SEC));
    MU_ASSERT_EQUAL(res, false);

    // retry after timeout (this time succeed)
    res = group->wait(dispatch_time(QDispatch::TimeNow, 5ull * NSEC_PER_SEC));
    MU_ASSERT_EQUAL(res, true);

    delete group;
    group = NULL;

    group = createGroup(100, 0);
    MU_ASSERT_NOT_NULL(group);

    group->notify(new GroupNotify, QDispatch::mainQueue());

    delete group;
    group = NULL;

    app.exec();

    MU_FAIL("Should never reach this");
    MU_END_TEST
}

#endif

// // Test the signal implementation

static uintptr_t* worker = 0;

class GroupTest : public QObject {

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

#ifdef XDISPATCH_HAS_BLOCKS

extern "C" void Qt_dispatch_group2(){
    char* argv = QString("test").toAscii().data();
    int argc = 1;
    QDispatchCoreApplication app(argc,&argv);

    MU_BEGIN_TEST(Qt_dispatch_group2);

    worker = new uintptr_t;
    *worker = 0;

    QDispatchGroup group;
    GroupTest* gt = new GroupTest;
    QObject::connect(&group, SIGNAL(allFinished()), gt, SLOT(notify()) );

    group.async(${
        QTest::qSleep(400);
        dispatch_atomic_inc(worker);
    });

    group.enableAllFinishedSignal();

    group.async(${
        QTest::qSleep(300);
        dispatch_atomic_inc(worker);
    });

    group.async(${
       QTest::qSleep(300);
        dispatch_atomic_inc(worker);
    });

    group.async(${
        QTest::qSleep(200);
        dispatch_atomic_inc(worker);
    });

    app.exec();
    MU_END_TEST;
}

#endif

#include <moc_Qt_dispatch_group.moc>

#endif
