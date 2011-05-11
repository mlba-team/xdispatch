
#include <qvariant.h>
#include <qatomic.h>
#include <qlist.h>
#include <qthread.h>
#include <qthreadstorage.h>
#include <qcoreapplication.h>

#include "../include/QtDispatch/qdispatchqueue.h"
#include "../include/QtDispatch/qdispatch.h"
#include "../include/QtDispatch/qblockrunnable.h"
#include "../include/QtDispatch/qdispatchsource.h"

QT_BEGIN_HEADER

/*
class QDispatchSourceThread : public QThread {

public:
	QDispatchSourceThread(){
		connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(terminate()));
	}

	void run(){
		exec();
	}

	static dispatch_once_t started;

};

dispatch_once_t QDispatchSourceThread::started = 0;
extern "C" static void qstart_source_thread(void* dt){
	QDispatchSourceThread** thread = static_cast<QDispatchSourceThread**>(dt);

	*thread = new QDispatchSourceThread();
	(*thread)->start();
} */

class QDispatchNotifySource : public QRunnable {

public:
	QDispatchNotifySource(QObject* obj, QThreadStorage<QObject*>* stor, QRunnable* h)
		: object(obj), storage(stor), handler(h) {
		Q_ASSERT(stor);
		Q_ASSERT(h);
	}

	void run(){
		if(!handler)
			return;

		storage->setLocalData(object);
		handler->run();
		if(handler->autoDelete())
			delete handler;
		storage->setLocalData(NULL);
	}

private:
	QObject* object;
	QThreadStorage<QObject*>* storage;
	QRunnable* handler;
};

class QDispatchSource::Private {
public:
	Private() : target(QDispatch::globalQueue()){
		//dispatch_once_f(&QDispatchSourceThread::started, &thread, qstart_source_thread);
	}

	QDispatchSourceType* type;
	// The suspend count. A value less than zero means suspended
	QAtomicInt suspend_ct;
	QDispatchQueue target;
	QRunnable* handler;
	//QDispatchSourceThread* thread;
	static QThreadStorage<QObject*> storage;
};

QThreadStorage<QObject*> QDispatchSource::Private::storage;

QDispatchSource::QDispatchSource(QDispatchSourceType* t) : d(new Private){
	Q_CHECK_PTR(d);
	Q_ASSERT(t);
	d->type = t;
	connect(t, SIGNAL(ready(QObject*)), this, SLOT(signal(QObject*)));
	//t->init(d->thread);
}

QDispatchSource::~QDispatchSource(){
	delete d->type;
	delete d;
}

void QDispatchSource::setTarget(const QDispatchQueue& q){
	d->target = q;
}

void QDispatchSource::setHandler(QRunnable* r){
	Q_ASSERT(r);
	d->handler = r;
}

#ifdef XDISPATCH_HAS_BLOCKS
void QDispatchSource::setHandler(dispatch_block_t b){
	setHandler(new QBlockRunnable(b));
}
#endif

QObject* QDispatchSource::data(){
	return Private::storage.localData();
}

void QDispatchSource::signal(QObject* obj){
	if(!d->handler)
		return;

	if(d->suspend_ct < 0)
		return;

	if(obj == NULL)
		obj = d->type;

	d->target.async(new QDispatchNotifySource(obj, &d->storage, d->handler));
}

void QDispatchSource::resume(){
	d->suspend_ct.ref();
}

void QDispatchSource::suspend(){
	d->suspend_ct.deref();
}

QT_END_HEADER