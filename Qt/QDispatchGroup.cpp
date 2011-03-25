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
#include "_DispatchEventQueueImpl.h"
#include "_DispatchQueueImpl.h"
#include "JavaUtils.h"

QT_BEGIN_NAMESPACE

class QDispatchGroup::Private {
public:
    Private() : obj(NULL), notify(NULL), queue(NULL), references(1), pending_jobs(0){}
	~Private() {
		if(notify)
			delete notify;
		if(queue)
			delete queue;
	}

	void runNotify(){
        if(!obj)
            return;

		emit obj->allFinished();
		if(queue && notify)
			queue->dispatch(notify);
	}

    QPointer<QDispatchGroup> obj;
	QRunnable* notify;
	QDispatchQueue* queue;
    QAtomicInt references;
    QAtomicInt pending_jobs;
};

class Q_GroupRunnable : public QRunnable {
public:
    Q_GroupRunnable(QRunnable* r, QDispatchGroup::Private* g) : QRunnable(), work(r), group(g) {
        group->references.ref();
        group->pending_jobs.ref();
    }

	QRunnable* work;
    QDispatchGroup::Private* group;

	virtual void run(){
		Q_ASSERT(work);
        Q_ASSERT(group);

		if(work){
			work->run();
			if(work->autoDelete())
				delete work;
		}

        if(!group->pending_jobs.deref())
            group->runNotify();

        if(!group->references.deref())
            delete group;
	};
};

QDispatchGroup::QDispatchGroup() : d(new Private()){
	d->obj = this;
}

QDispatchGroup::~QDispatchGroup(){
    if(!d->references.deref())
        delete d;
}

bool QDispatchGroup::wait(const QTime& t){
	return wait(QD->asDispatchTime(t));
}

bool QDispatchGroup::wait(dispatch_time_t t){
	dispatch_time_t now = 0;

    while(d->pending_jobs > 0 && t > now){
#ifdef WIN32
		Sleep(1);
#else
		usleep(100);
#endif
		now = dispatch_time(0,0);
	}

    return (d->pending_jobs == 0);
}

void QDispatchGroup::dispatch(QRunnable *r, QDispatchQueue *q){
	if(q==NULL)
		q = QD->getGlobalQueue();

    Q_ASSERT(r!=NULL);
    Q_ASSERT(q!=NULL);

    Q_GroupRunnable* gr = new Q_GroupRunnable(r, this->d);
	q->dispatch(gr);
}

void QDispatchGroup::notify(QRunnable *r, QDispatchQueue *q){
    d->notify = r;
    // this runnable will be reused, thus do not delete it
    r->setAutoDelete(false);
    d->queue = q==0 ? QD->getGlobalQueue() : q;

    if(d->pending_jobs == 0)
        d->runNotify();
}

#ifdef XDISPATCH_HAS_BLOCKS
void QDispatchGroup::dispatch(dispatch_block_t b, QDispatchQueue *q){
    QBlockRunnable* br = new QBlockRunnable(b);
    br->setAutoDelete(true);
    dispatch(br, q);
}

void QDispatchGroup::notify(dispatch_block_t b, QDispatchQueue* q){
	notify(new QBlockRunnable(b), q);
}
#endif

QDebug operator<<(QDebug dbg, const QDispatchGroup& g)
{	
    dbg.nospace() << "QDispatchGroup (" << g.d->pending_jobs << " pending, " << (g.d->notify==0 ? "no" : "has") << " notifier)";
	return dbg.space();
}

QT_END_NAMESPACE
