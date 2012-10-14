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

// static member variables, initialized during library initialization
static std::map<std::string, semaphore> user_lock_semaphores;
static synclock rw_lock;

void init_synchronized_feature() {
  // initialize the static variables manually
  // during library initialization to ensure
  // everything is set up properly on first use
  user_lock_semaphores = std::map<std::string, semaphore>();
  rw_lock = synclock();
}

synclock::synclock()
  : _semaphore(1), _lock_active(false) {}

synclock::synclock(const semaphore& s)
  : _semaphore(s), _lock_active(false) {

  XDISPATCH_ASSERT(s.native_semaphore ());
}

synclock::synclock(const synclock &other)
  : _semaphore(other._semaphore), _lock_active(false)  {
  // this could have ugly side-effects like the semaphore
  // getting assigned a new value but the old one not getting released
  // so we need to ensure that no lock is currently active
  XDISPATCH_ASSERT(! other._lock_active);
}

synclock&  synclock::operator = (const synclock& other) {
  // this could have ugly side-effects like the semaphore
  // getting assigned a new value but the old one not getting released
  // so we need to ensure that no lock is currently active
  XDISPATCH_ASSERT(! _lock_active);
  XDISPATCH_ASSERT(! other._lock_active)
  _semaphore = other._semaphore;

  return (*this);
}

synclock::~synclock() {
  // the _lock_active variable is
  // used as some kind of state variable
  // to notify if the current object is holding
  // the semaphore.
  // As such it has to be false on construction
  // and false on deletion, i.e. the calls to lock()
  // must be balanced to the calls of unlock()
  XDISPATCH_ASSERT(! _lock_active);
}

synclock::operator bool() const{
  return _lock_active;
}

void synclock::lock(){
  XDISPATCH_ASSERT(! _lock_active);

  // lock() - and unlock() are called from the same thread,
  // so no race condition here. Thread-safety is
  // provided by the used semaphores
  _lock_active = true;
  _semaphore.acquire ();
}

void synclock::unlock(){
  XDISPATCH_ASSERT(_lock_active);

  // lock() - and unlock() are called from the same thread,
  // so no race condition here. Thread-safety is
  // provided by the used semaphores
  _lock_active = false;
  _semaphore.release ();

}


void xdispatch::init_semaphore_for_synclock(void* dt){
  XDISPATCH_ASSERT( dt );
  semaphore* sem_ptr = (semaphore*)(dt);
  XDISPATCH_ASSERT( sem_ptr );

  (*sem_ptr) = semaphore(1);
}

synclock xdispatch::lock_area(const std::string& key){

  synclock sync_instance;

  if (user_lock_semaphores.count(key) != 0)
    sync_instance = synclock( user_lock_semaphores[ key ] );
  else {
    XDISPATCH_SYNC_HEADER( rw_lock ){
      // in the meantime the semaphore might have been created
      if(user_lock_semaphores.count(key) != 0)
        sync_instance = synclock( user_lock_semaphores[ key ] );
      else {
        // if not create it
        semaphore sem(1);
        user_lock_semaphores.insert( std::pair<std::string, semaphore>(key,sem) );
        sync_instance = synclock( sem );
      }
    }
  }

  sync_instance.lock ();
  return sync_instance;
}
