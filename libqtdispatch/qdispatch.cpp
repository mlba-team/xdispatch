/*
* Copyright (c) 2011-2013 MLBA-Team. All rights reserved.
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


#include <QTime>
#include <QString>
#include <QThread>
#include <QCoreApplication>

#include "../include/QtDispatch/qdispatch.h"

QT_BEGIN_NAMESPACE

QDispatch::QDispatch () { }


QDispatchQueue QDispatch::currentQueue()
{
    return QDispatchQueue( xdispatch::current_queue() );
}


QDispatchQueue QDispatch::globalQueue(
    Priority p
)
{
    xdispatch::queue_priority prio;

    switch( p )
    {
     case QDispatch::LOW:
         prio = xdispatch::LOW;
         break;

     case QDispatch::HIGH:
         prio = xdispatch::HIGH;
         break;

     default:
         prio = xdispatch::DEFAULT;
    }

    return QDispatchQueue( xdispatch::global_queue( prio ) );
} // globalQueue


QDispatchQueue QDispatch::mainQueue()
{
    return QDispatchQueue( xdispatch::main_queue() );
}


QTime QDispatch::asQTime(
    const xdispatch::time &t
)
{
    xdispatch::time xdt = t;

    int hours = static_cast< int > ( xdt ) / ( NSEC_PER_SEC * 3600 );

    xdt %= ( NSEC_PER_SEC * 3600 );
    int min = static_cast< int > ( xdt ) / ( NSEC_PER_SEC * 60 );
    xdt %= ( NSEC_PER_SEC * 60 );
    int sec = static_cast< int > ( xdt ) / ( NSEC_PER_SEC );
    xdt %= ( NSEC_PER_SEC );
    return QTime( hours, min, sec, xdt / NSEC_PER_MSEC );
}


xdispatch::time QDispatch::asDispatchTime(
    const QTime &t
)
{
    return dispatch_time( DISPATCH_TIME_NOW, QTime::currentTime().msecsTo( t ) * NSEC_PER_MSEC );
}


QT_END_NAMESPACE