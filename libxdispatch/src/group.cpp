/*
* group.cpp
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


#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE


group::group ()
    : object(),
      m_native( dispatch_group_create() )
{
    XDISPATCH_ASSERT( m_native );
}


group::group (
    dispatch_group_t g
)
    : object(),
      m_native( g )
{
    XDISPATCH_ASSERT( m_native );
    dispatch_retain( m_native );
}


group::group (
    const group &other
)
    : object( other ),
      m_native( other.m_native )
{
    XDISPATCH_ASSERT( m_native );
    dispatch_retain( m_native );
}


group::~group ()
{
    dispatch_release( m_native );
    m_native = 0;
}


void group::async(
    operation *r,
    const queue &q
)
{
    dispatch_queue_t nat_q = (dispatch_queue_t)q.native();

    XDISPATCH_ASSERT( nat_q );
    dispatch_group_async_f( m_native, nat_q, r, _xdispatch_run_operation );
}


bool group::wait(
    dispatch_time_t time
)
{
    return dispatch_group_wait( m_native, time ) == 0;
}


bool group::wait(
    struct tm *t
)
{
    return wait( as_dispatch_time( t ) );
}


void group::notify(
    operation *r,
    const queue &q
)
{
    dispatch_queue_t nat_q = (dispatch_queue_t)q.native();

    XDISPATCH_ASSERT( nat_q );
    dispatch_group_notify_f( m_native, nat_q, r, _xdispatch_run_operation );
}


dispatch_object_t group::native() const
{
    return m_native;
}


dispatch_group_t group::native_group() const
{
    return m_native;
}


group & group::operator = (
    const group &other
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
    }

    return *this;
}


std::ostream & xdispatch::operator << (
    std::ostream &stream,
    const group *q
)
{
    stream << "xdispatch::group";
    return stream;
}


std::ostream & xdispatch::operator << (
    std::ostream &stream,
    const group &q
)
{
    stream << "xdispatch::group";
    return stream;
}
