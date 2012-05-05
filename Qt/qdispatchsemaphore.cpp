/*
* Copyright (c) 2011-2012 MLBA-Team. All rights reserved.
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
#include <QDebug>

#include "../include/QtDispatch/qdispatch.h"
#include "../include/QtDispatch/qdispatchsemaphore.h"

QT_BEGIN_NAMESPACE

QDispatchSemaphore::QDispatchSemaphore(const QDispatchSemaphore &obj) : xdispatch::semaphore(obj){

}

QDispatchSemaphore::QDispatchSemaphore(int value) : xdispatch::semaphore(1){

}

QDispatchSemaphore::QDispatchSemaphore(dispatch_semaphore_t t) : xdispatch::semaphore(t){

}

QDispatchSemaphore::QDispatchSemaphore(const xdispatch::semaphore &obj) : xdispatch::semaphore(obj) {

}

QDispatchSemaphore::~QDispatchSemaphore(){

}

bool QDispatchSemaphore::tryAcquire(dispatch_time_t t){
    return try_acquire(t);
}

bool QDispatchSemaphore::tryAcquire(struct tm* t){
    return try_acquire(t);
}

bool QDispatchSemaphore::tryAcquire(const QTime& t){
    return try_acquire(QDispatch::asDispatchTime(t));
}

QDebug operator<<(QDebug dbg, const QDispatchSemaphore& s)
{	
	dbg.nospace() << "QDispatchSemaphore (no info available)";
	return dbg.space();
}

QT_END_NAMESPACE
