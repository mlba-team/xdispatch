/*
* queue.cpp
* 
* Copyright (c) 2011-2016 MLBA-Team
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

queue::queue (
    dispatch_queue_t q
)
    : m_native( q ),
      m_label( dispatch_queue_get_label( q ) )
{
    XDISPATCH_ASSERT( m_native );
    dispatch_retain( m_native );
}


queue::queue (
    const queue &other
)
    : m_native( other.m_native ),
      m_label( other.m_label )
{
    XDISPATCH_ASSERT( m_native );
    dispatch_retain( m_native );
}


queue::queue (
    const std::string &label
)
    : m_native( dispatch_queue_create( label.c_str(), NULL ) ),
      m_label( label )
{
    XDISPATCH_ASSERT( m_native );
}


queue::queue (
    const char *label
)
    : m_native( dispatch_queue_create( label, NULL ) ),
      m_label( label )
{
    XDISPATCH_ASSERT( m_native );
}


queue::~queue ()
{
    dispatch_release( m_native );
    m_native = 0;
}


void queue::async(
    operation *op
)
{
    dispatch_async_f( m_native, op, _xdispatch_run_operation );
}


void queue::apply(
    size_t times,
    iteration_operation *op
)
{
    iteration_wrap wrap( op, times );

    dispatch_apply_f( times, m_native, &wrap, _xdispatch_run_iter_wrap );
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
    dispatch_after_f( time, m_native, op, _xdispatch_run_operation );
}


void queue::sync(
    operation *op
)
{
    dispatch_sync_f( m_native, op, _xdispatch_run_operation );
}


void queue::finalizer(
    operation *op,
    const queue &q
)
{
    dispatch_set_finalizer_f( m_native, _xdispatch_run_operation );
    dispatch_set_context( m_native, op );
    dispatch_set_target_queue( m_native, (dispatch_queue_t)q.native() );
}


const std::string & queue::label() const
{
    return m_label;
}


dispatch_object_t queue::native() const
{
    return m_native;
}


dispatch_queue_t queue::native_queue() const
{
    return m_native;
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

        m_native = other.m_native;
        XDISPATCH_ASSERT( m_native );
        dispatch_retain( m_native );
        m_label = other.m_label;
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
