/*
* semaphore.cpp
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


#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

semaphore::semaphore (
    int val
)
    : m_native( dispatch_semaphore_create( val ) )
{
    XDISPATCH_ASSERT( m_native );
}


semaphore::semaphore (
    dispatch_semaphore_t sem
)
    : m_native( sem )
{
    XDISPATCH_ASSERT( m_native );
    dispatch_retain( m_native );
}


semaphore::semaphore (
    const semaphore &other
)
    : m_native( other.m_native )
{
    XDISPATCH_ASSERT( m_native );
    dispatch_retain( m_native );
}


semaphore::~semaphore ()
{
    if( m_native )
    {
        dispatch_release( m_native );
        m_native = 0;
    }
}


int semaphore::release()
{
    return dispatch_semaphore_signal( m_native );
}


void semaphore::acquire()
{
    dispatch_semaphore_wait( m_native, time_forever );
}


bool semaphore::try_acquire(
    dispatch_time_t time
)
{
    return dispatch_semaphore_wait( m_native, time ) == 0;
}


bool semaphore::try_acquire(
    struct tm *time
)
{
    return try_acquire( as_dispatch_time( time ) );
}


dispatch_semaphore_t semaphore::native_semaphore() const
{
    return m_native;
}


xdispatch::semaphore & semaphore::operator = (
    const semaphore &other
)
{
    if( *this != other )
    {
        m_native = other.m_native;
        XDISPATCH_ASSERT( m_native );
        dispatch_retain( m_native );
    }

    return *this;
}


bool semaphore::operator == (
    const semaphore &other
)
{
    return m_native == other.m_native;
}


bool semaphore::operator != (
    const semaphore &other
)
{
    return m_native != other.m_native;
}


bool semaphore::operator == (
    const dispatch_semaphore_t &other
)
{
    return m_native == other;
}


bool semaphore::operator != (
    const dispatch_semaphore_t &other
)
{
    return m_native != other;
}


std::ostream & xdispatch::operator << (
    std::ostream &stream,
    const semaphore *q
)
{
    stream << "xdispatch::semaphore";
    return stream;
}


std::ostream & xdispatch::operator << (
    std::ostream &stream,
    const semaphore &q
)
{
    stream << "xdispatch::semaphore";
    return stream;
}