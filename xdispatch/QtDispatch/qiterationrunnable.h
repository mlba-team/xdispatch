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



#ifndef QITERATIONRUNNABLE_H_
#define QITERATIONRUNNABLE_H_

#include <QRunnable>

/**
Provides a QRunnable Implementation for passing
a single parameter to the actual run() implementation.

This can for example be used in a case when for each iteration
of a loop a new QRunnable will be created and you want to know
during the execution of each runnable at which iteration it
was created.
*/
class QIterationRunnable : public QRunnable {

public:
	/**
	Constructs a new QBlockRunnable passing
	the given parameter.
	*/
	QIterationRunnable();
	QIterationRunnable(int index);
	QIterationRunnable(const QIterationRunnable&);
	~QIterationRunnable();
	virtual void run();
	virtual void run(int index) = 0;

private:
	class Private;
	Private* d;

};

#endif /* QITERATIONRUNNABLE_H_ */
