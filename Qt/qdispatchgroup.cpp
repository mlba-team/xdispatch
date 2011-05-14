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
#include <QPointer>
#include <QTime>
#include <QDebug>

#include "../include/QtDispatch/qdispatchqueue.h"
#include "../include/QtDispatch/qdispatchgroup.h"
#include "../include/QtDispatch/qdispatch.h"
#include "../include/QtDispatch/qblockrunnable.h"
#include "qrunnableoperations.h"

QT_BEGIN_NAMESPACE

class QDispatchGroup::Private {
public:
    Private() : obj(NULL), notify(NULL), references(2) {}
	~Private() {
        if(notify && notify->auto_delete())
			delete notify;
        if(queue)
            delete queue;
	}

	void runNotify(){
        if(queue && notify)
            queue->async(notify);

        if(obj)
            emit obj->allFinished();
	}

    static void notifier(void* dt){
        Q_ASSERT(dt);
        QDispatchGroup::Private* d = static_cast<QDispatchGroup::Private*>(dt);

        d->runNotify();
        if(!d->obj && !d->references.deref())
            delete d;
    }

    QPointer<QDispatchGroup> obj;
    xdispatch::operation* notify;
    QDispatchQueue* queue;
    QAtomicInt references;
};

QDispatchGroup::QDispatchGroup() : d(new Private()){
	d->obj = this;
}

QDispatchGroup::QDispatchGroup(dispatch_group_t o) : xdispatch::group(o), d(new Private()){
    d->obj = this;
}

QDispatchGroup::QDispatchGroup(const QDispatchGroup & obj) : xdispatch::group(obj), d(new Private()){
    d->obj = this;
}

QDispatchGroup::QDispatchGroup(const xdispatch::group & obj) : xdispatch::group(obj), d(new Private()){
    d->obj = this;
}

QDispatchGroup::~QDispatchGroup(){
    if(!d->references.deref())
        delete d;

}

bool QDispatchGroup::wait(const QTime& t){
    return wait(QDispatch::asDispatchTime(t));
}

void QDispatchGroup::async(QRunnable *r, const xdispatch::queue& q){
    Q_ASSERT(r);
    async(new RunnableOperation(r), q);
    dispatch_group_notify_f((dispatch_group_t)native(), (dispatch_queue_t)xdispatch::global_queue().native(), d, QDispatchGroup::Private::notifier);
}

void QDispatchGroup::async(xdispatch::operation * op, const xdispatch::queue & q){
    Q_ASSERT(op);
    xdispatch::group::async(op, q);
    dispatch_group_notify_f((dispatch_group_t)native(), (dispatch_queue_t)xdispatch::global_queue().native(), d, QDispatchGroup::Private::notifier);
}

void QDispatchGroup::notify(QRunnable *r, const xdispatch::queue& q){
    Q_ASSERT(r);
    notify(new RunnableOperation(r), q);

    dispatch_group_notify_f((dispatch_group_t)native(), (dispatch_queue_t)xdispatch::global_queue().native(), d, QDispatchGroup::Private::notifier);
}

void QDispatchGroup::notify(xdispatch::operation* op, const xdispatch::queue& q){
    Q_ASSERT(op);
    d->notify = op;
    d->queue = new QDispatchQueue(q);

    dispatch_group_notify_f((dispatch_group_t)native(), (dispatch_queue_t)xdispatch::global_queue().native(), d, QDispatchGroup::Private::notifier);
}

void QDispatchGroup::suspend(){
	xdispatch::group::suspend();
}

void QDispatchGroup::resume(){
	xdispatch::group::resume();
}

#ifdef XDISPATCH_HAS_BLOCKS

void QDispatchGroup::async(dispatch_block_t b, const xdispatch::queue & q){
    async(new QBlockRunnable(b), q);
}

void QDispatchGroup::notify(dispatch_block_t b, const xdispatch::queue& q){
    notify(new QBlockRunnable(b), q);
}

#endif

QDebug operator<<(QDebug dbg, const QDispatchGroup& g)
{	
    dbg.nospace() << "QDispatchGroup (" << (g.d->notify==0 ? "no" : "has") << " notifier)";
	return dbg.space();
}

QT_END_NAMESPACE
