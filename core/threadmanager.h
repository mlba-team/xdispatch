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

//
// @file threadmanager.h
// Manages all threads, i.e. which queue is currently
// running on which thread
// 

void _thread_man_init(void);
void _thread_man_cleanup(void);

//*
// The "timer" thread
// 
void* _timer_thread(void*);

//*
// Spawns a new timer thread
// 
void _spawn_timer_thread(void*);

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
