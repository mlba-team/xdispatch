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



#ifndef _DISPATCHEVENT_H_
#define _DISPATCHEVENT_H_

#include "QSharedPointer"
#include <QEvent>
#include <QRunnable>

class _DispatchEventData {
    
public:
    _DispatchEventData(QRunnable* r) : content(r) {}
    _DispatchEventData(const _DispatchEventData& other) : content(other.content) {}
    ~_DispatchEventData(){
        if(content->autoDelete()){
            delete content;
        }
    }
    
    QRunnable* content;
};

class _DispatchEvent : public QEvent {
public:
	_DispatchEvent(QRunnable*);
	_DispatchEvent(QRunnable*, int index);
    _DispatchEvent(const QSharedPointer<_DispatchEventData>&, int index);

	void exec();

	static QEvent::Type TYPECONSTANT;

private:
    int index;
	QSharedPointer<_DispatchEventData> d;
};

#endif /* _DISPATCHEVENT_H_ */
