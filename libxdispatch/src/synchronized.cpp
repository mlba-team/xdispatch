/*
* synchronized.cpp
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

#include <assert.h>
#include <map>
#include <string>
#include <utility>
#include <iostream>

#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

scope_lock::scope_lock (
    semaphore const &s
)
    : _semaphore( const_cast<semaphore&>( s ) ),
      _lock_active( true )
{
    XDISPATCH_ASSERT( s.native_semaphore() );
    _semaphore.acquire();
}


scope_lock::scope_lock (
    semaphore const * const s
)
    : _semaphore( *const_cast<semaphore* const >( s ) ),
      _lock_active( true )
{
    XDISPATCH_ASSERT( s->native_semaphore() );
    _semaphore.acquire();
}


scope_lock::~scope_lock ()
{
    // in a rare case there might an exception have
    // been thrown and due to this the scope_lock is
    // not unlocked properly. We need to handle this rare
    // case by testing for an exception and doing an unlock
    // during destruction.
    // FIXME: After a call to return within the owning loop this
    // is needed as well because no other call to unlock happens...
    if( _lock_active ) // && std::uncaught_exception())
    {
        release();
    }
    // the _lock_active variable is
    // used as some kind of state variable
    // to notify if the current object is holding
    // the semaphore.
    // As such it has to be false on construction
    // and false on deletion, i.e. the calls to lock()
    // must be balanced to the calls of unlock()
    XDISPATCH_ASSERT( !_lock_active );
}


scope_lock::operator bool () const
{
    return _lock_active;
}


void scope_lock::release()
{
    if( _lock_active )
    {
        _semaphore.release();
        _lock_active = false;
    }
}


void xdispatch::init_semaphore_for_scope_lock(
    void *dt
)
{
    XDISPATCH_ASSERT( dt );
    semaphore *sem_ptr = reinterpret_cast< semaphore * > ( dt );
    XDISPATCH_ASSERT( sem_ptr );

    ( *sem_ptr ) = semaphore( 1 );
}
