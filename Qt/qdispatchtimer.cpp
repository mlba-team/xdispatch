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
#include <QRunnable>
#include "../include/QtDispatch/qdispatch.h"
#include "../include/QtDispatch/qdispatchtimer.h"
#include "qrunnableoperations.h"

QT_BEGIN_NAMESPACE


using namespace xdispatch;

QDispatchTimer::QDispatchTimer(int msec, QObject* parent) : QObject(parent), timer(NSEC_PER_MSEC*msec){
    Q_ASSERT(msec > 0);
}

QDispatchTimer::QDispatchTimer(const xdispatch::timer& t, QObject* parent) : QObject(parent), timer(t){

}

QDispatchTimer::QDispatchTimer(const QDispatchTimer & other) : QObject(), timer(other){

}

QDispatchTimer::~QDispatchTimer(){

}

void QDispatchTimer::setInterval(int msec){
    timer::interval(msec* NSEC_PER_MSEC);
}

void QDispatchTimer::setTargetQueue(const xdispatch::queue& q){
    timer::target_queue(q);
}

void QDispatchTimer::setHandler(QRunnable* r){
    timer::handler(new RunnableOperation(r));
}

void QDispatchTimer::setLatency(int usec){
    timer::latency(NSEC_PER_USEC* usec);
}

void QDispatchTimer::singleShot(dispatch_time_t t, const xdispatch::queue& q, QRunnable* r){
    xdispatch::timer::single_shot(t, q, new RunnableOperation(r));
}

void QDispatchTimer::singleShot(const QTime& t, const xdispatch::queue& q, QRunnable* r){
    xdispatch::timer::single_shot(QDispatch::asDispatchTime(t), q, new RunnableOperation(r));
}


QDispatchTimer* QDispatchTimer::current(){
    timer* curr = timer::current();
    return static_cast<QDispatchTimer*>(curr);
}

void QDispatchTimer::start(){
    timer::start();
}

void QDispatchTimer::stop(){
    timer::stop();
}

Q_DISPATCH_EXPORT bool QDispatchTimer::operator ==(const QDispatchTimer& b) {
    return *(static_cast<timer*>(this)) == static_cast<timer>(b);
}

QT_END_NAMESPACE
