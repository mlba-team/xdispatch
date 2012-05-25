/*
* Copyright (c) 2011-2012 MLBA-Team. All rights reserved.
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

#include <assert.h>
#include <map>
#include <string>
#include <iostream>

#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE


synclock::synclock()
  : _semaphore(1), _lock_active(false) {}

synclock::synclock(dispatch_semaphore_t s)
  : _semaphore( s ), _lock_active(true) {

    XDISPATCH_ASSERT(s);
    _semaphore.acquire ();
}

synclock::synclock(const synclock &other)
  : _semaphore(other._semaphore), _lock_active(true)  {

    _semaphore.acquire ();
}

synclock::~synclock() {
    unlock();
}

synclock::operator bool() const{
    return _lock_active;
}

void synclock::unlock(){
    if(_lock_active) {
       _lock_active = false;
       _semaphore.release ();
    }
}


void xdispatch::init_semaphore_for_synclock(void* dt){
    XDISPATCH_ASSERT( dt );
    dispatch_semaphore_t* sem_ptr = (dispatch_semaphore_t*)(dt);
    XDISPATCH_ASSERT( sem_ptr );

    (*sem_ptr) = dispatch_semaphore_create(1);
    XDISPATCH_ASSERT( *sem_ptr );
}

static std::map<std::string, dispatch_semaphore_t> user_lock_semaphores;
static dispatch_semaphore_t rw_lock = dispatch_semaphore_create(1);

synclock xdispatch::get_lock_for_key(const std::string& key){
    dispatch_semaphore_t sem = NULL;

    if (user_lock_semaphores.count(key) != 0)
        sem = user_lock_semaphores[ key ];
    else {
        dispatch_semaphore_wait(rw_lock, time_forever);
        if(user_lock_semaphores.count(key) != 0)
			sem = user_lock_semaphores[ key ];
        else {
            sem = dispatch_semaphore_create(1);
            XDISPATCH_ASSERT(sem);
            user_lock_semaphores.insert( std::pair<std::string, dispatch_semaphore_t>(key,sem) );
        }
        dispatch_semaphore_signal(rw_lock);
    }

    return synclock(sem);
}
