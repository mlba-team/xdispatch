/*
 * Copyright (c) 2010 Apple Inc. All rights reserved.
 * Copyright (c) 2011 MLBA. All rights reserved.
 * Copyright (c) 2010 Mark Heily <mark@heily.com>
 *
 * @APPLE_APACHE_LICENSE_HEADER_START@
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
 * @APPLE_APACHE_LICENSE_HEADER_END@
 */

#ifndef THREADS_WIN_H_
#define THREADS_WIN_H_

#include "pthread_create.h"
#include "pthread_mutex.h"
#include "pthread_cond_variables.h"
#include "pthread_tls.h"
#include "posix_semaphore.h"

// currently this is borrowed from linux, we will see if this needs to get adapted
#define NSIG 32

/* Emulation of pthreads mutex functionality */
typedef CRITICAL_SECTION pthread_rwlock_t;
static int _cs_init(CRITICAL_SECTION* x){  InitializeCriticalSection(x); return 0; }
static int _cs_lock(CRITICAL_SECTION* x) {EnterCriticalSection (x); return 0; }
static int _cs_unlock(CRITICAL_SECTION* x) {LeaveCriticalSection(x); return 0; }
#define pthread_rwlock_rdlock _cs_lock
#define pthread_rwlock_wrlock _cs_lock
#define pthread_rwlock_unlock _cs_unlock
#define pthread_rwlock_init(x,y) _cs_init((x))

/* signals */
typedef int sigset_t;

#endif /* THREADS_WIN_H_ */
