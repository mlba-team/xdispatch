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
#include <QString>
#include <QThread>
#include <QCoreApplication>

#include "../include/QtDispatch/qdispatch.h"

QT_BEGIN_NAMESPACE

class QDispatch::Private {
public:
        Private() : highQueue(new QDispatchQueue(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH,0))),
                normalQueue(new QDispatchQueue(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,0))),
                lowQueue(new QDispatchQueue(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW,0))),
                mainQueue(new QDispatchQueue(dispatch_get_main_queue())) {}
	~Private(){ 
		delete mainQueue; 
		delete normalQueue;
		delete lowQueue;
		delete highQueue;
	}

	QDispatchQueue* highQueue, *normalQueue, *lowQueue;
	QDispatchQueue* mainQueue;
};

QDispatch* QDispatch::instance = new QDispatch();

QDispatch::QDispatch() : d(new Private){}

QDispatch::QDispatch(const QDispatch&){}

QDispatch::~QDispatch(){
	delete d;
}

QDispatchQueue* QDispatch::createQueue(const QString& label){
	try {
        return new QDispatchQueue(label);
	} catch(...) {
		return NULL;
	}
}

QDispatchQueue::a_ptr QDispatch::getCurrentQueue(){

    return QDispatchQueue::a_ptr(new QDispatchQueue(dispatch_get_current_queue() ));

}

QDispatchQueue* QDispatch::getGlobalQueue(Priority p){
	switch(p){
	case HIGH:
		return d->highQueue;
		break;
	case LOW:
		return d->lowQueue;
		break;
	default:
		return d->normalQueue;
		break;
	}
}

QDispatchQueue* QDispatch::getMainQueue(){
	return d->mainQueue;
}

QTime QDispatch::asQTime(dispatch_time_t t){
    int hours = (int)t / (NSEC_PER_SEC*3600);
    t %= (NSEC_PER_SEC*3600);
    int min = (int)t / (NSEC_PER_SEC*60);
    t %= (NSEC_PER_SEC*60);
    int sec = (int)t / (NSEC_PER_SEC);
    t %= (NSEC_PER_SEC);
    return QTime(hours, min, sec, t/NSEC_PER_MSEC);
}

dispatch_time_t QDispatch::asDispatchTime(const QTime & t){
    return dispatch_time(DISPATCH_TIME_NOW, QTime::currentTime().msecsTo(t)*NSEC_PER_MSEC);
}

QT_END_NAMESPACE
