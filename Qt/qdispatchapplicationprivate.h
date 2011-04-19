/*
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

#ifndef QDISPATCH_APPLICATIONPRIVATE_H_
#define QDISPATCH_APPLICATIONPRIVATE_H_

#include "../include/QtDispatch/qdispatchapplication.h"
#include "../include/xdispatch/dispatch.h"
#include <QDebug>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Dispatch)

class QProcessEventsOperation : public xdispatch::operation {
public:
    QProcessEventsOperation(QCoreApplication* app) : app(app) {
        Q_ASSERT(app);
    }

    void operator ()(){
        app->processEvents(QEventLoop::WaitForMoreEvents | QEventLoop::EventLoopExec);
        //qDebug() << "Processed events";
        xdispatch::current_queue().async(new QProcessEventsOperation(app));
    }

private:
    QCoreApplication* app;
};

class QDispatchApplicationPrivate {

public:
    QDispatchApplicationPrivate() : main(xdispatch::main_queue()), self(NULL) {
        Q_ASSERT(main.native());
    }

    xdispatch::queue main;
    QCoreApplication* self;

    int exec() {
        Q_ASSERT(self);
        Q_ASSERT(main.native());

        // add the first listen for events here
        main.async(new QProcessEventsOperation(self));

        // enter the dispatching loop
        xdispatch::exec();

        return 0;
    }

};

QT_END_NAMESPACE
QT_END_HEADER

#endif /* QDISPATCH_APPLICATIONPRIVATE_H_ */
