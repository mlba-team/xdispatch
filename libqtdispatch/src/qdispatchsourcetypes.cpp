/*
* qdispatchsourcetypes.cpp
*
* Copyright (c) 2011-2013 MLBA-Team.
* All rights reserved.
*
* @LICENSE_HEADER_START@
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
* @LICENSE_HEADER_END@
*/

#include <QIODevice>
#include <QVariant>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "../include/QtDispatch/qdispatchsource.h"

QT_BEGIN_NAMESPACE

QDispatchSourceTypeSignal::QDispatchSourceTypeSignal(
    QObject* sender,
    const char* signal
)
{
    connect( sender, signal, this, SIGNAL( ready() ), Qt::DirectConnection );
}


QDispatchSourceTypeSignal::~QDispatchSourceTypeSignal() { }


void QDispatchSourceTypeSignal::init(
    QThread* unused
) { }


QDispatchSourceTypeIODevice::QDispatchSourceTypeIODevice(
    QIODevice* d
)
    : dev( d )
{
    Q_ASSERT( d );
    connect( dev, SIGNAL( readyRead() ), this, SLOT( avail() ), Qt::DirectConnection );
}


QDispatchSourceTypeIODevice::~QDispatchSourceTypeIODevice() { }


void QDispatchSourceTypeIODevice::init(
    QThread* t
)
{
    dev->moveToThread( t );
}


void QDispatchSourceTypeIODevice::avail()
{
    emit ready( dev );
}


QDispatchSourceTypeIODeviceRead::QDispatchSourceTypeIODeviceRead(
    QIODevice* d
)
    : QDispatchSourceTypeIODevice( d ) { }


QDispatchSourceTypeIODeviceWrite::QDispatchSourceTypeIODeviceWrite(
    QIODevice* d
)
    : dev( d )
{
    Q_ASSERT( d );
    connect( dev, SIGNAL( bytesWritten( qint64 ) ), this, SLOT( finished( qint64 ) ), Qt::DirectConnection );
}


QDispatchSourceTypeIODeviceWrite::~QDispatchSourceTypeIODeviceWrite() { }


void QDispatchSourceTypeIODeviceWrite::init(
    QThread* t
)
{
    dev->moveToThread( t );
}


void QDispatchSourceTypeIODeviceWrite::finished(
    qint64
)
{
    emit ready( dev );
}


QDispatchSourceTypeNetworkManager::QDispatchSourceTypeNetworkManager(
    QNetworkAccessManager* man
)
    : manager( man )
{
    Q_ASSERT( man );
    connect( man, SIGNAL( finished( QNetworkReply* ) ), this, SLOT( finished( QNetworkReply* ) ), Qt::DirectConnection );
}


QDispatchSourceTypeNetworkManager::~QDispatchSourceTypeNetworkManager() { }


void QDispatchSourceTypeNetworkManager::finished(
    QNetworkReply* r
)
{
    emit ready( r );
}


void QDispatchSourceTypeNetworkManager::init(
    QThread* t
)
{
    manager->moveToThread( t );
}


QT_END_NAMESPACE