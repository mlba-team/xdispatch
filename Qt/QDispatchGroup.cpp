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

#include "../core/atomic.h"
#include "QDispatchQueue.h"
#include "QDispatchGroup.h"
#include "QDispatch.h"
#include "QBlockRunnable.h"
#include "_DispatchEventQueueImpl.h"
#include "_DispatchQueueImpl.h"
#include "JavaUtils.h"

class QDispatchGroup::Private {
public:
        Private() : obj(NULL), notify(NULL), queue(NULL), count(0){}
        Private(const Private& p) : notify(p.notify), queue(p.queue), count(p.count){

	}
	~Private() {
		if(notify)
			delete notify;
		if(queue)
			delete queue;
	}

	void runNotify(){
		emit obj->allFinished();
		if(queue && notify)
			queue->dispatch(notify);
	}

	QDispatchGroup* obj;
	QRunnable* notify;
	QDispatchQueue* queue;
	ATOMIC_INT count;
};

class _GroupRunnable : public QRunnable {
public:
#ifndef HAS_BLOCKS
	_GroupRunnable(QRunnable* r, QDispatchGroup* g) : QRunnable(), work(r), group(g) {}
#else
	_GroupRunnable(QRunnable* r, QDispatchGroup* g) : QRunnable(), work(r), block(NULL), group(g) {}
	_GroupRunnable(dispatch_block_t b, QDispatchGroup* g) : QRunnable(), work(NULL), block(new dispatch_block_t(b)), group(g) {}
#endif

	QRunnable* work;
#ifdef HAS_BLOCKS
	dispatch_block_t* block;
#endif
	QPointer<QDispatchGroup> group;

	virtual void run(){
#ifdef HAS_BLOCKS
		Q_ASSERT(work || block);
#else
		Q_ASSERT(work);
#endif
		if(work){
			work->run();
			if(work->autoDelete())
				delete work;
		}
#ifdef HAS_BLOCKS
		if(block){
			(*block)();
			delete block;
		}
#endif
		if(group && atomic_dec_get(&group->d->count)==0 ) {
			group->d->runNotify();
		}
	};
};

QDispatchGroup::QDispatchGroup(const QDispatchGroup& g) : d(new Private(*g.d)) {
	d->obj = this;
}

QDispatchGroup::QDispatchGroup() : d(new Private()){
	d->obj = this;
}

QDispatchGroup::~QDispatchGroup(){
	delete d;
}

bool QDispatchGroup::wait(const QTime& t){
	return wait(QD->asDispatchTime(t));
}

bool QDispatchGroup::wait(dispatch_time_t t){
	dispatch_time_t now = 0;

	while(d->count>0 && t > now){
#ifdef WIN32
		Sleep(1);
#else
		usleep(100);
#endif
		now = dispatch_time(0,0);
	}

	return (d->count==0);
}

void QDispatchGroup::dispatch(QRunnable *r, QDispatchQueue *q){
	if(q==NULL)
		q = QD->getGlobalQueue();

	Q_ASSERT(r!=NULL && q!=NULL);

	_GroupRunnable* gr = new _GroupRunnable(r, this);
	atomic_inc_get(&d->count);
	q->dispatch(gr);
}

#ifdef HAS_BLOCKS
void QDispatchGroup::dispatch(dispatch_block_t b, QDispatchQueue *q){
	if(q==NULL)
		q = QD->getGlobalQueue();

	Q_ASSERT(q!=NULL);

	_GroupRunnable* gr = new _GroupRunnable(b, this);
	atomic_inc_get(&d->count);
	q->dispatch(gr);
}

void QDispatchGroup::notify(dispatch_block_t b, QDispatchQueue* q){
	notify(new QBlockRunnable(b), q);
}
#endif

void QDispatchGroup::notify(QRunnable *r, QDispatchQueue *q){
	d->notify = r;
	// this runnable will be reused, thus do not delete it
	r->setAutoDelete(false);
	d->queue = q==0 ? QD->getGlobalQueue() : q;

	if(d->count == 0)
		d->runNotify();
}

QDebug operator<<(QDebug dbg, const QDispatchGroup& g)
{	
	dbg.nospace() << "QDispatchGroup (" << g.d->count << " pending, " << (g.d->notify==0 ? "no" : "has") << " notifier)";
	return dbg.space();
}
