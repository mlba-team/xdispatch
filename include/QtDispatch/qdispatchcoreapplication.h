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



#ifndef QDISPATCH_COREAPPLICATION_H_
#define QDISPATCH_COREAPPLICATION_H_

#include <QCoreApplication>
#include <QDebug>
#include "qdispatchglobal.h"
#include "../xdispatch/dispatch.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Dispatch)

class QDispatchApplicationPrivate;

class QDispatchCoreApplication : public QCoreApplication {

    Q_OBJECT

public:
    QDispatchCoreApplication(int& argc, char** argv);
    ~QDispatchCoreApplication();

    static int exec();

private:
    QDispatchApplicationPrivate* d;
    static QDispatchCoreApplication* self;

};

QT_END_NAMESPACE
QT_END_HEADER

#endif /* QDISPATCH_EVENTDISPATCHER_H_ */
