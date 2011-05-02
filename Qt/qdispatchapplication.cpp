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


#include <QThread>

#include "../include/QtDispatch/qdispatchapplication.h"
#include "qdispatchapplicationprivate.h"

#ifndef Q_OS_MAC

QT_BEGIN_NAMESPACE

QDispatchApplication* QDispatchApplication::self = NULL;

QDispatchApplication::QDispatchApplication(int& argc, char** argv) : QApplication(argc, argv), d(new QDispatchApplicationPrivate) {
    Q_CHECK_PTR(d);
    self = this;
    d->self = this;
}

QDispatchApplication::~QDispatchApplication() {
    delete d;
}

int QDispatchApplication::exec() {

    if(QThread::currentThread() != self->thread()) {
        qWarning("%s::exec: Must be called from the main thread", self->metaObject()->className());
        return -1;
    }

    return self->d->exec();

}

QT_END_NAMESPACE

#endif
