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



#include <QCoreApplication>
#include <QTimer>
#include <QTime>

#include "../xdispatch/QtDispatch/qblockrunnable.h"
#include "../xdispatch/QtDispatch/qiterationblockrunnable.h"
#include "../xdispatch/QtDispatch/qdispatch.h"
#include "_MainEventLoopHandler.h"
#include "_DispatchEvent.h"
#include "_DelayedDispatchEvent.h"
#include "_DispatchEventQueueImpl.h"

_DispatchEventQueueImpl::_DispatchEventQueueImpl() : QDispatchQueue(), finalizer(NULL), finalQueue(NULL) {

}

_DispatchEventQueueImpl::_DispatchEventQueueImpl(const _DispatchEventQueueImpl& obj)
    : QDispatchQueue(), finalizer(NULL), finalQueue(NULL) {

}

_DispatchEventQueueImpl::~_DispatchEventQueueImpl(){
    if(this->finalizer && this->finalQueue) {
        if(this == this->finalQueue) {
            this->finalizer->run();
            if(this->finalizer->autoDelete())
                delete this->finalizer;
        } else
            this->finalQueue->dispatch(this->finalizer);
    }
}

void _DispatchEventQueueImpl::dispatch(QRunnable * runnable){
	_DispatchEvent* e = new _DispatchEvent(runnable);
	qApp->postEvent(_MainEventLoopHandler::instance,e);
}

void _DispatchEventQueueImpl::dispatch(QIterationRunnable * runnable, int times){
    QSharedPointer<_DispatchEventData> dt(new _DispatchEventData(runnable));
	for(int i = 0; i < times; i++){
        _DispatchEvent* e = new _DispatchEvent(dt, i);
		qApp->postEvent(_MainEventLoopHandler::instance, e);
	}
}

void _DispatchEventQueueImpl::dispatchAfter(QRunnable *r, dispatch_time_t time){
	dispatchAfter(r, QD->asQTime(time));
}

void _DispatchEventQueueImpl::dispatchAfter(QRunnable *r, const QTime& time){
	_DelayedDispatchEvent* de = new _DelayedDispatchEvent(r);
	int delta = QTime::currentTime().msecsTo(time);
	Q_ASSERT(delta>0);
	QTimer::singleShot(delta, de, SLOT(exec()));
}

void _DispatchEventQueueImpl::dispatchSync(QRunnable * runnable){
	_DispatchEvent* e = new _DispatchEvent(runnable);
	qApp->postEvent(_MainEventLoopHandler::instance,e);
	// Note: Is this call really blocking until our event was handled?
	qApp->flush();
}

#ifdef HAS_BLOCKS
void _DispatchEventQueueImpl::dispatch(dispatch_block_t block){
	_DispatchEvent* e = new _DispatchEvent(new QBlockRunnable(block));
	qApp->postEvent(_MainEventLoopHandler::instance,e);
}

void _DispatchEventQueueImpl::dispatchSync(dispatch_block_t block){
	dispatchSync(new QBlockRunnable(block));
}

void _DispatchEventQueueImpl::dispatchAfter(dispatch_block_t b, dispatch_time_t time){
	dispatchAfter(b, QD->asQTime(time));
}

void _DispatchEventQueueImpl::dispatchAfter(dispatch_block_t block, const QTime& t){
	QRunnable* runnable = new QBlockRunnable(block);
	dispatchAfter(runnable, t);
}

void _DispatchEventQueueImpl::dispatch(dispatch_iteration_block_t block, int times){
	QIterationBlockRunnable* runnable = new QIterationBlockRunnable(block);
	dispatch(runnable, times);
}

void _DispatchEventQueueImpl::setFinalizer(dispatch_block_t b, QDispatchQueue* q){
    setFinalizer(new QBlockRunnable(b), q);
}

#endif

void _DispatchEventQueueImpl::suspend(){
    // on qt suspending or resuming the main loop does make no sense
}

void _DispatchEventQueueImpl::resume(){

}

const QString _DispatchEventQueueImpl::label() const {
	return QString("com.mlba.xdispatch.main-qtqueue");
}

void _DispatchEventQueueImpl::setFinalizer(QRunnable* r, QDispatchQueue* dq){

    if(dq == NULL)
        dq = QD->getGlobalQueue();

    this->finalizer = r;
    this->finalQueue = dq;
}
