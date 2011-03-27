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

#ifdef WIN32
#	include <Windows.h>
#else
#	include <unistd.h>
#endif

#include "../include/QtDispatch/qdispatchqueue.h"
#include "../include/QtDispatch/qdispatchgroup.h"
#include "../include/QtDispatch/qdispatch.h"
#include "../include/QtDispatch/qblockrunnable.h"
#include "qrunnableoperations.h"
#include "JavaUtils.h"

QT_BEGIN_NAMESPACE

class QDispatchGroup::Private {
public:
    Private() : obj(NULL), notify(NULL), references(1), pending_jobs(0){}
	~Private() {
        if(notify && notify->auto_delete())
			delete notify;
        if(queue)
            delete queue;
	}

	void runNotify(){
        if(queue && notify)
            queue->async(notify);

        if(!obj)
            return;

        emit obj->all_finished();
	}

    QPointer<QDispatchGroup> obj;
    xdispatch::operation* notify;
    QDispatchQueue* queue;
    QAtomicInt references;
    QAtomicInt pending_jobs;
};

QDispatchGroup::QDispatchGroup() : d(new Private()){
	d->obj = this;
}

QDispatchGroup::QDispatchGroup(dispatch_group_t o) : xdispatch::group(o), d(new Private()){
    d->obj = this;
}

QDispatchGroup::~QDispatchGroup(){
    if(!d->references.deref())
        delete d;
}

bool QDispatchGroup::wait(const QTime& t){
	return wait(QD->asDispatchTime(t));
}

void QDispatchGroup::async(QRunnable *r, xdispatch::queue *q){
	if(q==NULL)
		q = QD->getGlobalQueue();

    Q_ASSERT(r!=NULL);
    Q_ASSERT(q!=NULL);

    async(new RunnableOperation(r), q);
}

void QDispatchGroup::notify(QRunnable *r, xdispatch::queue *q){
    notify(new RunnableOperation(r), q);
}

void QDispatchGroup::notify(xdispatch::operation* op, xdispatch::queue* q){
    d->notify = op;
    d->queue = new QDispatchQueue(q==0 ? *xdispatch::dispatch::instance->global_queue() : *q);

    xdispatch::group::notify(new xdispatch::ptr_operation<QDispatchGroup::Private>(d, &QDispatchGroup::Private::runNotify), q);
}

#ifdef XDISPATCH_HAS_BLOCKS

void QDispatchGroup::notify(dispatch_block_t b, xdispatch::queue* q){
    notify(new QBlockRunnable(b), q);
}

#endif

QDebug operator<<(QDebug dbg, const QDispatchGroup& g)
{	
    dbg.nospace() << "QDispatchGroup (" << g.d->pending_jobs << " pending, " << (g.d->notify==0 ? "no" : "has") << " notifier)";
	return dbg.space();
}

QT_END_NAMESPACE
