/*
* timer.cpp
*
* Copyright (c) 2011-2017 MLBA-Team
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

#include <map>
#include <stdlib.h>
#include <stdio.h>

#include "xdispatch_internal.h"
#include "source_internal.h"

__XDISPATCH_USE_NAMESPACE


class timer_type
    : public native_source
{
public:
    timer_type (
        timer *parent,
        uint64_t interval,
        dispatch_time_t first,
        dispatch_queue_t q
    )
        : native_source( dispatch_source_create( DISPATCH_SOURCE_TYPE_TIMER, 0, 0, q ) ),
          _parent( parent ),
          _interval( interval ),
          _latency( 0 )
    {
        XDISPATCH_ASSERT( _parent );

        dispatch_source_set_timer( native(), first, interval, 0 );
    }

    void interval(
        uint64_t i
    )
    {
        _interval = i;
        update_timeout();
    }

    void latency(
        uint64_t l
    )
    {
        _latency = l;
        update_timeout();
    }

    void on_source_ready()
    {
        ready( _parent );
    }

protected:
    inline void update_timeout()
    {
        dispatch_source_set_timer( native(), time_now, _interval, _latency );
    }

private:
    timer *_parent;
    uint64_t _interval;
    uint64_t _latency;
};


timer::timer (
    uint64_t i,
    const xdispatch::queue &q,
    const xdispatch::time &first
)
    : source( new timer_type( this, i, as_native_dispatch_time( first ), q.native_queue() ) ) { }


timer::timer (
    const timer &t
)
    : source( t ) { }


timer::~timer (){ }


void timer::interval(
    uint64_t i
)
{
    static_cast< timer_type * > ( source::source_type() )->interval( i );
}


void timer::latency(
    uint64_t l
)
{
    static_cast< timer_type * > ( source::source_type() )->latency( l );
}


timer * timer::current()
{
    return source::data< timer * > ();
}


void timer::single_shot(
    dispatch_time_t t,
    const xdispatch::queue &q,
    xdispatch::operation *op
)
{
    xdispatch::queue( q ).after( t, op );
}


void timer::single_shot(
    struct tm *t,
    const xdispatch::queue &q,
    xdispatch::operation *op
)
{
    xdispatch::queue( q ).after( t, op );
}


timer & timer::operator = (
    const timer &other
)
{
    if( *this != other )
    {
        source::operator = (
            other
        );
    }

    return *this;
}