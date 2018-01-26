/*
* qdispatchgroup.cpp
*
* Copyright (c) 2011-2018 MLBA-Team
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


#include <QRunnable>
#include <QPointer>
#include <QTime>
#include <QDebug>
#include <QObject>

#include "../include/QtDispatch/qdispatchqueue.h"
#include "../include/QtDispatch/qdispatchgroup.h"
#include "../include/QtDispatch/qdispatch.h"
#include "../include/QtDispatch/qblockrunnable.h"
#include "qrunnableoperations.h"

QT_BEGIN_NAMESPACE

class QDispatchGroup::Private
    : public QObject
{
    Q_OBJECT


public:
    Private ()
        : QObject() { }


    void emitFinished()
    {
        emit groupFinished();
    }

signals:
    void groupFinished();
};


class QDispatchGroup::Emitter
    : public xdispatch::operation
{
public:
    Emitter (
        QDispatchGroup::Private *p
    )
        : operation(),
          _runnable( NULL ),
          _operation( NULL ),
          _group( p ) { }


    Emitter (
        QRunnable *qr,
        QDispatchGroup::Private *p
    )
        : operation(),
          _runnable( qr ),
          _operation( NULL ),
          _group( p ) { }


    Emitter (
        xdispatch::operation *op,
        QDispatchGroup::Private *p
    )
        : operation(),
          _runnable( NULL ),
          _operation( op ),
          _group( p ) { }


    virtual ~Emitter ()
    {
        if( _runnable && _runnable->autoDelete() )
            delete _runnable;

        if( _operation && _operation->auto_delete() )
            delete _operation;
    }

    void operator () ()
    {
        if( _runnable )
            _runnable->run();

        if( _operation )
            ( *_operation )();

        if( _group )
            _group->emitFinished();
    }

private:
    QRunnable *_runnable;
    xdispatch::operation *_operation;
    QDispatchGroup::Private *_group;
};


QDispatchGroup::QDispatchGroup ()
    : d( new Private() )
{
    connect( d.data(), SIGNAL( groupFinished() ), this, SIGNAL( allFinished() ) );
}


QDispatchGroup::QDispatchGroup (
    dispatch_group_t o
)
    : xdispatch::group( o ),
      d( new Private() )
{
    connect( d.data(), SIGNAL( groupFinished() ), this, SIGNAL( allFinished() ) );
}


QDispatchGroup::QDispatchGroup (
    const QDispatchGroup &obj
)
    : xdispatch::group( obj ),
      d( new Private() )
{
    connect( d.data(), SIGNAL( groupFinished() ), this, SIGNAL( allFinished() ) );
}


QDispatchGroup::QDispatchGroup (
    const xdispatch::group &obj
)
    : xdispatch::group( obj ),
      d( new Private() )
{
    connect( d.data(), SIGNAL( groupFinished() ), this, SIGNAL( allFinished() ) );
}


QDispatchGroup::~QDispatchGroup (){ }


void QDispatchGroup::enableAllFinishedSignal()
{
    notify( new Emitter( d.data() ), xdispatch::main_queue() );
}


bool QDispatchGroup::wait(
    const QTime &t
)
{
    if( t.msec() == 0 )
        return wait( DISPATCH_TIME_FOREVER );
    else
        return wait( QDispatch::asDispatchTime( t ) );
}


void QDispatchGroup::async(
    QRunnable *r,
    const xdispatch::queue &q
)
{
    Q_ASSERT( r );

    async( new RunnableOperation( r ), q );
}


void QDispatchGroup::notify(
    QRunnable *r,
    const xdispatch::queue &q
)
{
    Q_ASSERT( r );

    xdispatch::group::notify( new Emitter( r, d.data() ), q );
}


void QDispatchGroup::notify(
    xdispatch::operation *op,
    const xdispatch::queue &q
)
{
    Q_ASSERT( op );

    xdispatch::group::notify( new Emitter( op, d.data() ), q );
}


void QDispatchGroup::suspend()
{
    xdispatch::group::suspend();
}


void QDispatchGroup::resume()
{
    xdispatch::group::resume();
}


QDebug operator << (
    QDebug dbg,
    const QDispatchGroup &g
)
{
    dbg.nospace() << "QDispatchGroup (no details available)";
    return dbg.space();
}


#include <qdispatchgroup.moc>

QT_END_NAMESPACE