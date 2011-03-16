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


#include <QTime>

#include "../xdispatch/QtDispatch/qdispatch.h"
#include "../xdispatch/QtDispatch/qdispatchsemaphore.h"

class QDispatchSemaphore::Private {
public:
	Private(long v) : sem(dispatch_semaphore_create(v)) {}
	Private(const Private& p) : sem(p.sem) {
		dispatch_retain(sem);
	}
	~Private(){ 
		dispatch_release(sem); 
	}

	dispatch_semaphore_t sem;
};

QDispatchSemaphore::QDispatchSemaphore(const QDispatchSemaphore &obj) : d(new Private(*obj.d)){

}

QDispatchSemaphore::QDispatchSemaphore(int value) : d(new Private(value)){

}

QDispatchSemaphore::~QDispatchSemaphore(){
	delete d;
}

bool QDispatchSemaphore::tryAcquire(dispatch_time_t t) {
	return dispatch_semaphore_wait(d->sem, t)==0;
}

bool QDispatchSemaphore::tryAcquire(const QTime& t){
	return tryAcquire(QD->asDispatchTime(t));
}

int QDispatchSemaphore::release(){
	return dispatch_semaphore_signal(d->sem);
}

QDebug operator<<(QDebug dbg, const QDispatchSemaphore& s)
{	
	dbg.nospace() << "QDispatchSemaphore (no info available)";
	return dbg.space();
}
