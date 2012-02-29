
#include <qvariant.h>
#include <qatomic.h>
#include <qlist.h>
#include <qthread.h>
#include <qthreadstorage.h>
#include <qcoreapplication.h>

#include "../core/platform/platform.h"

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

template <typename T> class QDispatchThreadStorage {

public:
    QDispatchThreadStorage(){
       if(pthread_key_create(&_tls, NULL))
           throw "Unable to create tls";
    }
    ~QDispatchThreadStorage(){
    }

    void setLocalData(T* o){
        pthread_setspecific(_tls, o);
    }
    T* localData(){
        return (T*)pthread_getspecific(_tls);
    }

private:
    pthread_key_t _tls;

};

class QDispatchNotifySource : public QRunnable {

public:
	QDispatchNotifySource(QObject* obj, QDispatchThreadStorage<QObject>* stor, QRunnable* h)
		: object(obj), storage(stor), handler(h) {
		Q_ASSERT(stor);
		Q_ASSERT(h);
	}

	void run(){
		if(!handler)
			return;

		storage->setLocalData(object);
		handler->run();
		storage->setLocalData(NULL);
	}

private:
	QObject* object;
	QDispatchThreadStorage<QObject>* storage;
	QRunnable* handler;
};

class QDispatchSource::Private {
public:
	Private() : target(QDispatch::globalQueue()){
		//dispatch_once_f(&QDispatchSourceThread::started, &thread, qstart_source_thread);
	}
    ~Private(){
        delete type;
        if(handler->autoDelete())
			delete handler;
    }

	QDispatchSourceType* type;
	// The suspend count. A value less than zero means suspended
	QAtomicInt suspend_ct;
	QDispatchQueue target;
	QRunnable* handler;
	//QDispatchSourceThread* thread;
	static QDispatchThreadStorage<QObject> storage;
};

QDispatchThreadStorage<QObject> QDispatchSource::Private::storage;

QDispatchSource::QDispatchSource(QDispatchSourceType* t) : d(new Private){
	Q_CHECK_PTR(d);
	Q_ASSERT(t);
	d->type = t;
	connect(t, SIGNAL(ready(QObject*)), this, SLOT(signal(QObject*)), Qt::DirectConnection);
	//t->init(d->thread);
}

QDispatchSource::~QDispatchSource(){

}

void QDispatchSource::setTargetQueue(const QDispatchQueue& q){
	d->target = q;
}

QDispatchQueue QDispatchSource::targetQueue() const {
    return d->target;
}

void QDispatchSource::setHandler(QRunnable* r){
	Q_ASSERT(r);
	d->handler = r;
}

QObject* QDispatchSource::_data(){
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
