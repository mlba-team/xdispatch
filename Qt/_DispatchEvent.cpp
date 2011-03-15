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



#include "QIterationRunnable.h"
#include "_DispatchEvent.h"
#include "../core/atomic.h"

QEvent::Type _DispatchEvent::TYPECONSTANT = QEvent::Type(registerEventType(QEvent::User+2));

_DispatchEvent::_DispatchEvent(QRunnable* r) : QEvent(TYPECONSTANT), content(r), index(-1), count(NULL) {
}

_DispatchEvent::_DispatchEvent(QRunnable* r, int index, ATOMIC_INT* count) : QEvent(TYPECONSTANT), content(r), index(index), count(count) {
}

_DispatchEvent::~_DispatchEvent(){
	if(content!=NULL && content->autoDelete())
		delete content;
}

void _DispatchEvent::exec(){

	if(index < 0) {
		content->run();
		if(content->autoDelete()) {
			delete content;
			content = NULL;
		}
	} else {
        Q_ASSERT(count);
		static_cast<QIterationRunnable*>(content)->run(index);
		if(atomic_dec_get(count)==0){
			if(content->autoDelete()) {
				delete content;
				content = NULL;
			}
            qDebug("Free count");
			delete count;
            count = 0;
		}
	}
}
