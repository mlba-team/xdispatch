/*
* Qt_dispatch_source_network_lambda.cpp
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
#include <QtCore/QDebug>
#include <QtDispatch/QtDispatch>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "Qt_tests.h"

/*
 Tests the source type network manager
 */

extern "C" void Qt_dispatch_source_network_lambda(){
    char argv[] = "test";
    int argc = 1;
    QDispatchApplication app(argc, (char**)&argv);

        MU_BEGIN_TEST(Qt_dispatch_source_network_lambda);

	// configure the source
	QNetworkAccessManager* man = new QNetworkAccessManager();
	QDispatchSource src(new QDispatchSourceTypeNetworkManager(man));
	src.setHandler([=]{

		QNetworkReply* r = QDispatchSource::data<QNetworkReply>();
		MU_ASSERT_NOT_NULL(r);
		

		QString content = r->readAll();
		if(content.isEmpty())
			content = (r->readAll());
		MU_MESSAGE("Received response contains:");
		qDebug() << "\t\t" << content;

		r->deleteLater();
		MU_PASS("");
	});

	// now post a request
	man->get(QNetworkRequest(QUrl("http://www.google.com")));

	app.exec();
	MU_END_TEST;
}
