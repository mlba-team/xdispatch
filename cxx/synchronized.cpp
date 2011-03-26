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

synclock::synclock() : lock(false) {
    sem = dispatch_semaphore_create(1);
    assert(sem!=NULL);
}

synclock::synclock(dispatch_semaphore_t s) : sem(s), lock(true) {
    assert(s);
    dispatch_retain(sem);
    dispatch_semaphore_wait(sem,DISPATCH_TIME_FOREVER);
}

synclock::synclock(const synclock &other) : sem(other.sem), lock(true)  {
    assert(sem);
    dispatch_retain(sem);
    dispatch_semaphore_wait(sem,DISPATCH_TIME_FOREVER);
}

synclock::~synclock() {
    unlock();
    dispatch_release(sem);
}

synclock::operator bool() const{
    return lock;
}

void synclock::unlock(){
    if(lock) {
        lock = false;
        dispatch_semaphore_signal(sem);
    }
}

static dispatch_semaphore_t rw_lock = dispatch_semaphore_create(1);
static std::map<unsigned int, dispatch_semaphore_t> lock_semaphores;

synclock xdispatch::get_lock_for_key(unsigned int key){
    dispatch_semaphore_t sem = NULL;

    if (lock_semaphores.count(key) != 0)
        sem = lock_semaphores[key];
    else {
        dispatch_semaphore_wait(rw_lock,DISPATCH_TIME_FOREVER);
        if(lock_semaphores.count(key) != 0)
            sem = lock_semaphores[key];
        else {
            sem = dispatch_semaphore_create(1);
            assert(sem);
            lock_semaphores.insert( std::pair<unsigned int, dispatch_semaphore_t>(key,sem) );
        }
        dispatch_semaphore_signal(rw_lock);
    }

    return synclock(sem);
}
