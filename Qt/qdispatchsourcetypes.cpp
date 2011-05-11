

#include <qiodevice.h>
#include <qvariant.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>

#include "../include/QtDispatch/qdispatchsource.h"

QT_BEGIN_NAMESPACE

QDispatchSourceTypeSignal::QDispatchSourceTypeSignal(QObject* sender, const char* signal){
	connect(sender, signal, this, SIGNAL(ready()));
}

QDispatchSourceTypeSignal::~QDispatchSourceTypeSignal(){

}

void QDispatchSourceTypeSignal::init(QThread* unused){

}



QDispatchSourceTypeIODevice::QDispatchSourceTypeIODevice(QIODevice* d) : dev(d){
	Q_ASSERT(d);
	connect(dev, SIGNAL(readyRead()), this, SLOT(avail()));
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
	connect(dev, SIGNAL(bytesWritten(qint64)), this, SLOT(finished(qint64)));
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
	connect(man, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
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