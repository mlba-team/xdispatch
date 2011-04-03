/*
* Copyright (c) 2010 Apple Inc. All rights reserved.
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


#ifndef DISPATCH_INTERNAL_H_
#define DISPATCH_INTERNAL_H_

#ifndef __DISPATCH_BEGIN_DECLS
# define __DISPATCH_BEGIN_DECLS
#endif
#ifndef __DISPATCH_END_DECLS
# define __DISPATCH_END_DECLS
#endif

// the configuration file
#include <core/config.h>

// common headers
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread_workqueue.h>
#ifdef HAVE_KQUEUE_H
# include <sys/event.h>
#endif
#ifdef HAVE_LIBKQUEUE_H
# include <kqueue/sys/event.h>
#endif

#include <assert.h>

/* I wish we had __builtin_expect_range() */
#if __GNUC__
# define fastpath(x)	((typeof(x))__builtin_expect((long)(x), ~0l))
# define slowpath(x)	((typeof(x))__builtin_expect((long)(x), 0l))
#else
# define fastpath(x) (x)
# define slowpath(x) (x)
#endif

#ifdef __cplusplus
 extern "C" {
#endif

// debug helpers
#include "debug.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// workaround 6368156
#ifdef NSEC_PER_SEC
#undef NSEC_PER_SEC
#endif
#ifdef USEC_PER_SEC
#undef USEC_PER_SEC
#endif
#ifdef NSEC_PER_USEC
#undef NSEC_PER_USEC
#endif
#ifdef NSEC_PER_MSEC
#undef NSEC_PER_MSEC
#endif
#define NSEC_PER_SEC (uint64_t)1000000000
#define NSEC_PER_MSEC (uint64_t)1000000
#define USEC_PER_SEC (uint64_t)1000000
#define NSEC_PER_USEC (uint64_t)1000

// the core api is marked as exported
#ifdef _WIN32
# define DISPATCH_EXPORT __declspec(dllexport)
#else
# define DISPATCH_EXPORT __attribute__((visibility("default")))
#endif

// include all necessary hardware / os workarounds
#ifdef _WIN32
# include "shim/os_windows.h"
# include "shim/threads_windows.h"
# include "shim/malloc_zone.h"
#elif __APPLE__
# include "shim/os_darwin.h"
# include "shim/threads_posix.h"
#else
# include "shim/os_posix.h"
# include "shim/threads_posix.h"
# include "shim/malloc_zone.h"
#endif
#include "shim/atomic.h"
#include "shim/hardware.h"
#include "shim/getprogname.h"
#include "shim/time.h"
#include "shim/perfmon.h"

#if USE_KEVENTS
# include "kevent_internal.h"
#endif

#ifdef __BLOCKS__
# include <Block.h>
# include "shim/Block_private.h"
#endif

#define DISPATCH_API_VERSION 20090501

#ifndef __DISPATCH_INDIRECT__
#define __DISPATCH_INDIRECT__
#endif

#include "../include/libdispatch/base.h"
#include "../include/libdispatch/object.h"
#include "../include/libdispatch/time.h"
#include "../include/libdispatch/queue.h"
/* DISABLED UNTIL CLEAN IMPLEMENTATION IS AVAILABLE
#include "source.h"
   */
#include "../include/libdispatch/group.h"
#include "../include/libdispatch/semaphore.h"
#include "../include/libdispatch/source.h"
#include "../include/libdispatch/once.h"

#include "events.h"
#include "execution.h"
#include "blocks.h"
#include "threadmanager.h"
#include "continuation_cache.h"
#include "object_internal.h"
#include "queue_private.h"
#include "semaphore_internal.h"
#include "queue_internal.h"
#include "source_internal.h"
#include "source_private.h"

// the global queues
extern dispatch_queue_t _dispatch_global_q[];
void _dispatch_root_queues_init();
extern bool _dispatch_safe_fork;

// other global  variables
struct _dispatch_hw_config_s {
    uint32_t cc_max_active;
    uint32_t cc_max_logical;
    uint32_t cc_max_physical;
};

extern bool _dispatch_safe_fork;
extern struct _dispatch_hw_config_s _dispatch_hw_config;

// some internally used funtions
struct timespec _dispatch_time_to_spec(dispatch_time_t t);
dispatch_time_t _dispatch_spec_to_time(const struct timespec* s);
uint64_t _dispatch_timeout(dispatch_time_t when);
struct timespec _dispatch_timeout_ts(dispatch_time_t when);
uint64_t _dispatch_get_nanoseconds(void);
dispatch_semaphore_t _dispatch_get_thread_semaphore(void);
void _dispatch_put_thread_semaphore(dispatch_semaphore_t);
bool _dispatch_source_testcancel(dispatch_source_t);
uint64_t _dispatch_timeout(dispatch_time_t when);
struct timespec _dispatch_timeout_ts(dispatch_time_t when);
void libdispatch_init(void);

#undef DISPATCH_INTERNAL_H_

#ifdef __cplusplus
 }
#endif

#endif /* QUEUE_INTERNAL_H_ */
