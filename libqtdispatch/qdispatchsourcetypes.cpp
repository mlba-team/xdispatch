

#include <QIODevice>
#include <QVariant>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "../include/QtDispatch/qdispatchsource.h"

QT_BEGIN_NAMESPACE

QDispatchSourceTypeSignal::QDispatchSourceTypeSignal(QObject* sender, const char* signal){
	connect(sender, signal, this, SIGNAL(ready()), Qt::DirectConnection);
}

QDispatchSourceTypeSignal::~QDispatchSourceTypeSignal(){

}

void QDispatchSourceTypeSignal::init(QThread* unused){

}



QDispatchSourceTypeIODevice::QDispatchSourceTypeIODevice(QIODevice* d) : dev(d){
	Q_ASSERT(d);
	connect(dev, SIGNAL(readyRead()), this, SLOT(avail()), Qt::DirectConnection);
}

QDispatchSourceTypeIODevice::~QDispatchSourceTypeIODevice(){

}

void QDispatchSourceTypeIODevice::init(QThread* t){
	dev->moveToThread(t);
}

void QDispatchSourceTypeIODevice::avail(){
	emit ready(dev);
}


QDispatchSourceTypeIODeviceRead::QDispatchSourceTypeIODeviceRead(QIODevice* d) : QDispatchSourceTypeIODevice(d) {}


QDispatchSourceTypeIODeviceWrite::QDispatchSourceTypeIODeviceWrite(QIODevice* d) : dev(d){
	Q_ASSERT(d);
	connect(dev, SIGNAL(bytesWritten(qint64)), this, SLOT(finished(qint64)), Qt::DirectConnection);
}

QDispatchSourceTypeIODeviceWrite::~QDispatchSourceTypeIODeviceWrite(){

}

void QDispatchSourceTypeIODeviceWrite::init(QThread* t){
	dev->moveToThread(t);
}

void QDispatchSourceTypeIODeviceWrite::finished(qint64){
	emit ready(dev);
}


QDispatchSourceTypeNetworkManager::QDispatchSourceTypeNetworkManager(QNetworkAccessManager* man) : manager(man){
	Q_ASSERT(man);
	connect(man, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)), Qt::DirectConnection);
}

QDispatchSourceTypeNetworkManager::~QDispatchSourceTypeNetworkManager(){

}

void QDispatchSourceTypeNetworkManager::finished(QNetworkReply* r){
	emit ready(r);
}

void QDispatchSourceTypeNetworkManager::init(QThread* t){
	manager->moveToThread(t);
}

QT_END_NAMESPACE
