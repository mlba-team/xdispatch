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

#include "../xdispatch/QtDispatch/qiterationrunnable.h"
#include "_DispatchEvent.h"
#include "../core/atomic.h"

QEvent::Type _DispatchEvent::TYPECONSTANT = QEvent::Type(registerEventType(QEvent::User+2));

_DispatchEvent::_DispatchEvent(QRunnable* r) : QEvent(TYPECONSTANT) , index(-1), d(new _DispatchEventData(r)) {
}

_DispatchEvent::_DispatchEvent(QRunnable* r, int index) : QEvent(TYPECONSTANT), index(index), d(new _DispatchEventData(r)) {
}

_DispatchEvent::_DispatchEvent(const QSharedPointer<_DispatchEventData>& data, int index) : QEvent(TYPECONSTANT), index(index), d(data) {
}

void _DispatchEvent::exec(){
    Q_ASSERT(d->content);
    
	if(index < 0) {
		d->content->run();
	} else {
        Q_ASSERT(index >= 0);
		QIterationRunnable* iter_run = dynamic_cast<QIterationRunnable*>(d->content);
        Q_ASSERT(iter_run != NULL);
        iter_run->run(index);
    }
}
