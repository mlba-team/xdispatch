/*
* queue.cpp
* 
* Copyright (c) 2011-2014 MLBA-Team
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


#include <string>

#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

class queue::data
{
public:
    data (){ }


    data (
        const data &other
    )
        : native( other.native ),
          label( other.label )
    {
        XDISPATCH_ASSERT( native );
        dispatch_retain( native );
    }

    ~data ()
    {
        if( native )
            dispatch_release( native );
    }

    dispatch_queue_t native;
    std::string label;
};


queue::queue (
    dispatch_queue_t q
)
    : object(),
      d( new data )
{
    XDISPATCH_ASSERT( d.get() );
    dispatch_retain( q );
    d->native = q;
    XDISPATCH_ASSERT( d->native );
    d->label = std::string( dispatch_queue_get_label( q ) );
}


queue::queue (
    const queue &other
)
    : object( other ),
      d( new data( *other.d ) )
{
    XDISPATCH_ASSERT( d.get() );
}


queue::queue (
    const std::string &label
)
    : object(),
      d( new data )
{
    XDISPATCH_ASSERT( d.get() );
    d->native = dispatch_queue_create( label.c_str(), NULL );
    XDISPATCH_ASSERT( d->native );
    d->label = label;
}


queue::queue (
    const char *label
)
    : object(),
      d( new data )
{
    XDISPATCH_ASSERT( label );
    XDISPATCH_ASSERT( d.get() );
    d->native = dispatch_queue_create( label, NULL );
    XDISPATCH_ASSERT( d->native );
    d->label = std::string( label );
}


queue::~queue (){ }


void queue::async(
    operation *op
)
{
    dispatch_async_f( d->native, op, _xdispatch_run_operation );
}


void queue::apply(
    size_t times,
    iteration_operation *op
)
{
    iteration_wrap wrap( op, times );

    dispatch_apply_f( times, d->native, &wrap, _xdispatch_run_iter_wrap );
}


void queue::after(
    struct tm *time,
    operation *op
)
{
    after( as_dispatch_time( time ), op );
}


void queue::after(
    dispatch_time_t time,
    operation *op
)
{
    dispatch_after_f( time, d->native, op, _xdispatch_run_operation );
}


void queue::sync(
    operation *op
)
{
    dispatch_sync_f( d->native, op, _xdispatch_run_operation );
}


void queue::finalizer(
    operation *op,
    const queue &q
)
{
    dispatch_set_finalizer_f( d->native, _xdispatch_run_operation );
    dispatch_set_context( d->native, op );
    dispatch_set_target_queue( d->native, (dispatch_queue_t)q.native() );
}


const std::string & queue::label() const
{
    return d->label;
}


dispatch_object_t queue::native() const
{
    return d->native;
}


dispatch_queue_t queue::native_queue() const
{
    return d->native;
}


xdispatch::queue & queue::operator = (
    const queue &other
)
{
    if( *this != other )
    {
        object::operator = (
            other
        );

        d = pointer< data >::unique( new data( *other.d ) );
        XDISPATCH_ASSERT( d.get() );
    }

    return *this;
}


std::ostream & xdispatch::operator << (
    std::ostream &stream,
    const queue *q
)
{
    stream << "xdispatch::queue (" << q->label() << ")";
    return stream;
}


std::ostream & xdispatch::operator << (
    std::ostream &stream,
    const queue &q
)
{
    stream << "xdispatch::queue (" << q.label() << ")";
    return stream;
}