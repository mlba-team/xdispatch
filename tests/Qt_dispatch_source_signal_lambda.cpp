/*
* Qt_dispatch_source_signal_lambda.cpp
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

#include <QtCore/QTime>
#include <QtCore/QObject>
#include <QtDispatch/QtDispatch>

#include "Qt_tests.h"

/*
 Tests the source type signal
 */

class EmitterLambda : public QObject {

	Q_OBJECT

public:
	void notify(){
		emit ready();
	}

signals:
	void ready();

};

extern "C" void Qt_dispatch_source_signal_lambda(){
    QTDISPATCH_decl_argv_argc;
    QDispatchApplication app(argc, argv);

    MU_BEGIN_TEST(Qt_dispatch_source_signal_lambda);

    EmitterLambda object;

	// configure the source
	QDispatchSource src(new QDispatchSourceTypeSignal(&object, SIGNAL(ready())));
    src.setTargetQueue(QDispatch::globalQueue(QDispatch::LOW));
	src.setHandler([=]{
		MU_MESSAGE("Signal was emitted");
		if(QDispatch::currentQueue().native() == dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0))
			MU_MESSAGE("Executed on low global queue");
		else if(QDispatch::currentQueue().native() == dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0))
			MU_MESSAGE("Executed on default global queue");
		else if(QDispatch::currentQueue().native() == dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0))
			MU_MESSAGE("Executed on high global queue");
		else if(QDispatch::currentQueue().native() == dispatch_get_main_queue())
			MU_MESSAGE("Executed on main queue");
		MU_ASSERT_EQUAL_HEX(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0), QDispatch::currentQueue().native());
		MU_PASS("");
	});
    src.resume();

	// trigger the signal
	object.notify();

	app.exec();
	MU_END_TEST;
}

#include <moc_Qt_dispatch_source_signal_lambda.moc>
