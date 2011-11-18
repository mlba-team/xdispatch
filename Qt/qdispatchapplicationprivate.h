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

#ifndef QDISPATCH_APPLICATIONPRIVATE_H_
#define QDISPATCH_APPLICATIONPRIVATE_H_

#include "../include/xdispatch/dispatch.h"
#include <QDebug>
#include <QEvent>

extern "C" {
	/* copied from libdispatch/private.h */
	typedef void (_dispatch_main_q_handler_4QT)(void);
	DISPATCH_EXPORT void _dispatch_main_queue_callback_4QT();
	DISPATCH_EXPORT void _dispatch_register_signal_handler_4QT(_dispatch_main_q_handler_4QT);
}

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QDispatchEvent : public QEvent {
public:
	QDispatchEvent() : QEvent(TYPECONSTANT) {}

	static QEvent::Type TYPECONSTANT;
};

class QDispatchLibBridge : public QObject {

public:
	QDispatchLibBridge() : QObject(NULL) {}

	bool event(QEvent* e){
		if(e->type() != QDispatchEvent::TYPECONSTANT)
			return false;

		_dispatch_main_queue_callback_4QT();
		return true;
	}

	static void handleNewItem(){
		qApp->postEvent(QDispatchLibBridge::instance, new QDispatchEvent());
	}

	static void registerCallback(){
		QDispatchEvent::TYPECONSTANT = QEvent::Type(QEvent::registerEventType(QEvent::User+1));
		instance = new QDispatchLibBridge();
		_dispatch_register_signal_handler_4QT(handleNewItem);

                // there might be another job pending
                // make sure that it is processed
                handleNewItem();
	}

	static void removeCallback(){
		delete instance;
	}

	static QDispatchLibBridge* instance;

private:
};

QT_END_NAMESPACE
QT_END_HEADER

#endif /* QDISPATCH_APPLICATIONPRIVATE_H_ */
