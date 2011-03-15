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



#ifndef THREADMANAGER_H_
#define THREADMANAGER_H_

#include "threadpool.h"

//
// @file threadmanager.h
// Manages all threads, i.e. controls the number of running
// threads and suspends or wakes them if needed.
// 

//
// The "main" of a thread
// 
void* _thread(void*);

//*
// The "timer" thread
// 
void* _timer_thread(void*);

//*
// Spawns a new thread
// 
void _spawn_thread(_thread_t t);

//*
// Spawns a new timer thread
// 
void _spawn_timer_thread(void*);

//
// Returns the best number of threads to use
// at the moment
// defaults to the number of processors on the system
// 
unsigned int _ideal_thread_count();

//
// Tests if an additional thread should be spawned
// 
void _check_threads(_threadpool_t p);

//
// Sets the queue the thread is currently working on
// 
void _set_thread_queue(dispatch_queue_t q);

//
// Returns the queue the thread is currently working on
// 
dispatch_queue_t _get_thread_queue();

#ifdef WIN32

#include <Windows.h>

struct timezone
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz);

#endif

#endif /* THREADMANAGER_H_ */
