/*
* Copyright (c) 2011 MLBA. All rights reserved.
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

struct synclock::data {
    dispatch_semaphore_t sem;
    bool lock;

    data(dispatch_semaphore_t s) : sem(s), lock(false) {
        XDISPATCH_ASSERT(s);
        dispatch_retain(s);
    }

    ~data(){
        dispatch_release(sem);
    }
};

synclock::synclock() : _d( new data(dispatch_semaphore_create(1)) ) {
    XDISPATCH_ASSERT(_d);
}

synclock::synclock(dispatch_semaphore_t s) : _d( new data(s) ) {
    XDISPATCH_ASSERT(_d);
    _d->lock = true;

    XDISPATCH_ASSERT(s);
    dispatch_semaphore_wait(_d->sem,DISPATCH_TIME_FOREVER);
}

synclock::synclock(const synclock &other) : _d( new data(other._d->sem) )  {
    XDISPATCH_ASSERT(_d);
    _d->lock = true;

    dispatch_semaphore_wait(_d->sem,DISPATCH_TIME_FOREVER);
}

synclock::~synclock() {
    unlock();
    delete _d;
}

synclock::operator bool() const{
    return _d->lock;
}

void synclock::unlock(){
    if(_d->lock) {
        _d->lock = false;
        dispatch_semaphore_signal(_d->sem);
    }
}

static dispatch_semaphore_t rw_lock = dispatch_semaphore_create(1);
static std::map<unsigned int, dispatch_semaphore_t> lock_semaphores;

synclock xdispatch::get_lock_for_key(unsigned int key){
    dispatch_semaphore_t sem = NULL;

    if (lock_semaphores.count(key) != 0)
        sem = lock_semaphores[key];
    else {
        dispatch_semaphore_wait(rw_lock, time_forever);
        if(lock_semaphores.count(key) != 0)
            sem = lock_semaphores[key];
        else {
            sem = dispatch_semaphore_create(1);
            XDISPATCH_ASSERT(sem);
            lock_semaphores.insert( std::pair<unsigned int, dispatch_semaphore_t>(key,sem) );
        }
        dispatch_semaphore_signal(rw_lock);
    }

    return synclock(sem);
}

static std::map<std::string, dispatch_semaphore_t> user_lock_semaphores;

synclock xdispatch::get_lock_for_key(const std::string& key){
    dispatch_semaphore_t sem = NULL;

    if (user_lock_semaphores.count(key) != 0)
        sem = user_lock_semaphores.at(key);
    else {
        dispatch_semaphore_wait(rw_lock, time_forever);
        if(user_lock_semaphores.count(key) != 0)
            sem = user_lock_semaphores.at(key);
        else {
            sem = dispatch_semaphore_create(1);
            XDISPATCH_ASSERT(sem);
            user_lock_semaphores.insert( std::pair<std::string, dispatch_semaphore_t>(key,sem) );
        }
        dispatch_semaphore_signal(rw_lock);
    }

    return synclock(sem);
}
