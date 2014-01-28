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

// static member variables, initialized during library initialization
static std::map< std::string, semaphore > user_lock_semaphores;
static synclock rw_lock;

void init_synchronized_feature()
{
    // initialize the static variables manually
    // during library initialization to ensure
    // everything is set up properly on first use
    user_lock_semaphores = std::map< std::string, semaphore > ();
    rw_lock = synclock();
}


synclock::synclock ()
    : _semaphore( 1 ),
      _lock_active( false ) { }


synclock::synclock (
    const semaphore &s,
    const bool auto_lock
)
    : _semaphore( s ),
      _lock_active( false )
{
    XDISPATCH_ASSERT( s.native_semaphore() );

    if( auto_lock )
        this->lock();
}


synclock::synclock (
    const synclock &other,
    const bool auto_lock
)
    : _semaphore( other._semaphore ),
      _lock_active( false )
{
    // this could have ugly side-effects like the semaphore
    // getting assigned a new value but the old one not getting released
    // so we need to ensure that no lock is currently active
    XDISPATCH_ASSERT( !other._lock_active );

    if( auto_lock )
        this->lock();
}


synclock::synclock (
    const synclock *other,
    const bool auto_lock
)
    : _semaphore( other->_semaphore ),
      _lock_active( false )
{
    XDISPATCH_ASSERT( other );
    // this could have ugly side-effects like the semaphore
    // getting assigned a new value but the old one not getting released
    // so we need to ensure that no lock is currently active
    XDISPATCH_ASSERT( !other->_lock_active );

    if( auto_lock )
        this->lock();
}


synclock::synclock (
    const std::string &key,
    const bool auto_lock
)
    : _semaphore( 1 ),
      _lock_active( false )
{
    if( user_lock_semaphores.count( key ) != 0 )
        _semaphore = user_lock_semaphores[ key ];
    else
    {
        XDISPATCH_SYNC_HEADER( rw_lock )
        {
            // in the meantime the semaphore might have been created
            if( user_lock_semaphores.count( key ) != 0 )
                _semaphore = user_lock_semaphores[ key ];
            else
            {
                // if not create it
                user_lock_semaphores.insert( std::pair< std::string, semaphore > ( key, _semaphore ) );
            }
        }
    }

    if( auto_lock )
        this->lock();
}


synclock & synclock::operator = (
    const synclock &other
)
{
    // this could have ugly side-effects like the semaphore
    // getting assigned a new value but the old one not getting released
    // so we need to ensure that no lock is currently active
    XDISPATCH_ASSERT( !_lock_active );
    XDISPATCH_ASSERT( !other._lock_active )
    _semaphore = other._semaphore;

    return *this;
}


synclock::~synclock ()
{
    // in a rare case there might an exception have
    // been thrown and due to this the synclock is
    // not unlocked properly. We need to handle this rare
    // case by testing for an exception and doing an unlock
    // during destruction.
    // FIXME: After a call to return within the owning loop this
    // is needed as well because no other call to unlock happens...
    if( _lock_active ) // && std::uncaught_exception())
        this->unlock();

    // the _lock_active variable is
    // used as some kind of state variable
    // to notify if the current object is holding
    // the semaphore.
    // As such it has to be false on construction
    // and false on deletion, i.e. the calls to lock()
    // must be balanced to the calls of unlock()
    XDISPATCH_ASSERT( !_lock_active );
}


synclock::operator bool () const
{
    return _lock_active;
}


synclock & synclock::lock()
{
    XDISPATCH_ASSERT( !_lock_active );

    // lock() - and unlock() are called from the same thread,
    // so no race condition here. Thread-safety is
    // provided by the used semaphores
    _semaphore.acquire();
    _lock_active = true;

    return *this;
}


synclock & synclock::unlock()
{
    XDISPATCH_ASSERT( _lock_active );

    // lock() - and unlock() are called from the same thread,
    // so no race condition here. Thread-safety is
    // provided by the used semaphores
    _lock_active = false;
    _semaphore.release();

    return *this;
}


void xdispatch::init_semaphore_for_synclock(
    void *dt
)
{
    XDISPATCH_ASSERT( dt );
    semaphore *sem_ptr = reinterpret_cast< semaphore * > ( dt );
    XDISPATCH_ASSERT( sem_ptr );

    ( *sem_ptr ) = semaphore( 1 );
}