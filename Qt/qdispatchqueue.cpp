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


#include <QRunnable>
#include <QString>
#include <QDebug>
#include "../include/QtDispatch/qdispatch.h"
#include "../include/QtDispatch/qblockrunnable.h"
#include "../include/QtDispatch/qiterationblockrunnable.h"
#include "qrunnableoperations.h"

QT_BEGIN_NAMESPACE

QDispatchQueue::QDispatchQueue(const QString &label) : xdispatch::queue(label.toStdString()) {

}

QDispatchQueue::QDispatchQueue(const char * label) : xdispatch::queue(label) {

}

QDispatchQueue::QDispatchQueue(dispatch_queue_t dq) : xdispatch::queue(dq){
    if(dq==NULL)
        throw("Cannot construct queue");
}

QDispatchQueue::QDispatchQueue(const QDispatchQueue& obj) : xdispatch::queue(obj){

}

QDispatchQueue::QDispatchQueue(const xdispatch::queue & obj) : xdispatch::queue(obj){

}

QDispatchQueue::~QDispatchQueue(){

}

void QDispatchQueue::async(QRunnable * runnable){
    Q_ASSERT(runnable);
    async(new RunnableOperation(runnable));
}

void QDispatchQueue::apply(QIterationRunnable * runnable, int times){
    Q_ASSERT(runnable);
    apply(new IterationRunnableOperation(runnable), times);
}

void QDispatchQueue::after(QRunnable *runnable, const QTime& t){
    Q_ASSERT(runnable);
    after(runnable, QDispatch::asDispatchTime(t));
}

void QDispatchQueue::after(QRunnable* r, dispatch_time_t t){
    Q_ASSERT(r);
    after(new RunnableOperation(r), t);
}

void QDispatchQueue::sync(QRunnable * runnable){
    Q_ASSERT(runnable);
    sync(new RunnableOperation(runnable));
}

void QDispatchQueue::setFinalizer(xdispatch::operation* op, const xdispatch::queue& q){
    finalizer(op, q);
}

#ifdef XDISPATCH_HAS_BLOCKS
void QDispatchQueue::setFinalizer(dispatch_block_t b, const xdispatch::queue& q){
    finalizer(b, q);
}
#endif

void QDispatchQueue::setFinalizer(QRunnable* r, const xdispatch::queue& dq){
    Q_ASSERT(r);

    finalizer(new RunnableOperation(r), dq);
}

void QDispatchQueue::setTarget(const xdispatch::queue& q){
    target_queue(q);
}

void QDispatchQueue::suspend(){
	xdispatch::queue::suspend();
}

void QDispatchQueue::resume(){
	xdispatch::queue::resume();
}

#ifdef XDISPATCH_HAS_BLOCKS
void QDispatchQueue::after(dispatch_block_t block, const QTime& t){
    after(block, QDispatch::asDispatchTime(t));
}
#endif

QDispatchQueue& QDispatchQueue::operator=(const QDispatchQueue& other){
	if(this != &other){
		xdispatch::queue::operator=(other);
	}
	return *this;
}

Q_DECL_EXPORT QDebug operator<<(QDebug dbg, const QDispatchQueue* q)
{
    dbg.nospace() << "QDispatchQueue (" << q->label().c_str() << ")";
    return dbg.space();
}
Q_DECL_EXPORT QDebug operator<<(QDebug dbg, const QDispatchQueue& q)
{
    dbg.nospace() << "QDispatchQueue (" << q.label().c_str() << ")";
    return dbg.space();
}

QT_END_NAMESPACE
