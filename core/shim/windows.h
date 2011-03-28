/*
* Copyright (c) 2011 MLBA. All rights reserved.
* Copyright (c) 2010 Mark Heily <mark@heily.com>
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

#ifndef SHIM_WINDOWS_H_
#define SHIM_WINDOWS_H_

#include <Windows.h>
#include "stdint.h"

#define inline __inline

// memory leak detection on windows
#if defined(_WIN32) && defined(DEBUG)
#	define _CRTDBG_MAP_ALLOC
#	include <stdlib.h>
#	include <crtdbg.h>
#endif

#define sleep(sec) 	Sleep(1000*sec)
#define strdup(p)	_strdup(p)
#define random()	rand()

typedef HANDLE pthread_t;

/* Emulation of pthreads mutex functionality */
/* (copied from libkqueue by mark heily) */
#define PTHREAD_PROCESS_SHARED 1
#define PTHREAD_PROCESS_PRIVATE 2
typedef CRITICAL_SECTION pthread_mutex_t;
typedef CRITICAL_SECTION pthread_spinlock_t;
typedef CRITICAL_SECTION pthread_rwlock_t;
#define _cs_init(x)  InitializeCriticalSection((x))
#define _cs_lock(x)  EnterCriticalSection ((x))
#define _cs_unlock(x)  LeaveCriticalSection ((x))
#define pthread_mutex_lock _cs_lock
#define pthread_mutex_unlock _cs_unlock
#define pthread_mutex_init(x,y) _cs_init((x))
#define pthread_spin_lock _cs_lock
#define pthread_spin_unlock _cs_unlock
#define pthread_spin_init(x,y) _cs_init((x))
#define pthread_mutex_init(x,y) _cs_init((x))
#define pthread_rwlock_rdlock _cs_lock
#define pthread_rwlock_wrlock _cs_lock
#define pthread_rwlock_unlock _cs_unlock
#define pthread_rwlock_init(x,y) _cs_init((x))

#endif /* SHIM_WINDOWS_H_ */
