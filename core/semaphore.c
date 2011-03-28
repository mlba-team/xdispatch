/*
' Copyright (c) 2008-2009 Apple Inc. All rights reserved.
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


#include "queue_internal.h"

dispatch_semaphore_t
dispatch_semaphore_create(long value){
        dispatch_semaphore_t sem = (dispatch_semaphore_t)_get_empty_object();

        sem_init(&cast_sem(sem)->lock, 0, value);
        return sem;
}

long
dispatch_semaphore_wait(dispatch_semaphore_t dsema, dispatch_time_t timeout){
    //struct timespec out = _dispatch_time_to_spec(timeout);

    return sem_wait(&cast_sem(dsema)->lock);
}

long
dispatch_semaphore_signal(dispatch_semaphore_t dsema){
        return sem_post(&cast_sem(dsema)->lock);
}
