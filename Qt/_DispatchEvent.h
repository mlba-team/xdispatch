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

#include <QEvent>
#include "../core/atomic.h"

class QRunnable;

class _DispatchEvent : public QEvent {
public:
	_DispatchEvent(QRunnable*);
	_DispatchEvent(QRunnable*, int index, ATOMIC_INT* count);
	~_DispatchEvent();

	void exec();

	static QEvent::Type TYPECONSTANT;

private:
	QRunnable* content;
	int index;
	ATOMIC_INT* count;
};

#endif /* _DISPATCHEVENT_H_ */
