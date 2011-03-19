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

#include <QDebug>
#include <QMutex>

#include "../xdispatch/QtDispatch/qdispatch.h"
#include "../xdispatch/QtDispatch/qdispatchsemaphore.h"
#include "../xdispatch/QtDispatch/qsynchronized.h"

QT_BEGIN_NAMESPACE

QDispatchSynclock::QDispatchSynclock() : lock(false) {
    sem = dispatch_semaphore_create(1);
    Q_ASSERT(sem!=NULL);
}

QDispatchSynclock::QDispatchSynclock(dispatch_semaphore_t s) : sem(s), lock(true) {
    Q_ASSERT(s);
    dispatch_retain(sem);
    Q_ASSERT(dispatch_semaphore_wait(sem,DISPATCH_TIME_FOREVER)==0);
}

QDispatchSynclock::QDispatchSynclock(const QDispatchSynclock &other) : sem(other.sem), lock(true)  {
    Q_ASSERT(sem);
    dispatch_retain(sem);
    Q_ASSERT(dispatch_semaphore_wait(sem,DISPATCH_TIME_FOREVER)==0);
}

QDispatchSynclock::~QDispatchSynclock() {
    unlock();
    dispatch_release(sem);
}

QDispatchSynclock::operator bool() const{
    return lock;
}

void QDispatchSynclock::unlock(){
    if(lock) {
        lock = false;
        dispatch_semaphore_signal(sem);
    }
}

static QMutex rw_lock;
static QHash<uint, dispatch_semaphore_t> lock_semaphores;

QDispatchSynclock qGetLockForKey(uint key){
    dispatch_semaphore_t sem = NULL;

    if (lock_semaphores.contains(key))
        sem = lock_semaphores.value(key);
    else {
        rw_lock.lock();
        if(lock_semaphores.contains(key))
            sem = lock_semaphores.value(key);
        else {
            sem = dispatch_semaphore_create(1);
            Q_ASSERT(sem);
            lock_semaphores.insert(key,sem);
        }
        rw_lock.unlock();
    }

    return QDispatchSynclock(sem);
}

QT_END_NAMESPACE
