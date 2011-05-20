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

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#ifndef __DISPATCH_BEGIN_DECLS
# define __DISPATCH_BEGIN_DECLS
#endif
#ifndef __DISPATCH_END_DECLS
# define __DISPATCH_END_DECLS
#endif

/* the configuration file */
#include <config/config.h>

/* common headers */
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <pthread_workqueue.h>
#ifdef HAVE_LIBKQUEUE_H
# include <kqueue/sys/event.h>
#else
# include <sys/event.h>
#endif

#include <assert.h>

/* I wish we had __builtin_expect_range() */
#if __GNUC__ && !defined(__STDC__)
# define fastpath(x)	((typeof(x))__builtin_expect((long)(x), ~0l))
# define slowpath(x)	((typeof(x))__builtin_expect((long)(x), 0l))
#else
# define fastpath(x) (x)
# define slowpath(x) (x)
#endif

/* debug helpers */
#include "debug.h"

/* workaround 6368156 */
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

/* the core api is marked as exported */
#ifndef DISPATCH_EXPORT
# ifdef _WIN32
#  ifdef DISPATCH_MAKEDLL
#   define DISPATCH_EXPORT __declspec(dllexport)
#  else
#   define DISPATCH_EXPORT __declspec(dllimport)
#  endif
# else
#  define DISPATCH_EXPORT __attribute__((visibility("default")))
# endif
#endif

/* include all necessary hardware / os workarounds
 * as early as possible to prevent namespace pollution by libdispatch
 */
#include "../platform/platform.h"

/* now include the libdispatch headers */
#define DISPATCH_API_VERSION 20090501

#ifndef __DISPATCH_INDIRECT__
#define __DISPATCH_INDIRECT__
#endif

#include "../../include/libdispatch/base.h"
#include "../../include/libdispatch/object.h"
#include "../../include/libdispatch/time.h"
#include "../../include/libdispatch/queue.h"
#include "../../include/libdispatch/source.h"
#include "../../include/libdispatch/group.h"
#include "../../include/libdispatch/semaphore.h"
#include "../../include/libdispatch/source.h"
#include "../../include/libdispatch/once.h"

/* and the last missing shims */
#include "shims/malloc_zone.h"
#include "shims/tsd.h"
#include "shims/atomic.h"
#include "shims/hardware.h"
#include "shims/getprogname.h"
#include "shims/time.h"
#include "shims/perfmon.h"

#if USE_KEVENTS
# include "kevent_internal.h"
#endif

#ifdef __BLOCKS__
# include <Block.h>
# include "shims/Block_private.h"
#endif

#include "blocks.h"
#include "continuation_cache.h"
#include "object_internal.h"
#include "semaphore_internal.h"
#include "queue_internal.h"
#include "source_internal.h"

/* private.h uses #include_next and must be included last to avoid picking
 * up installed headers. */
#include "queue_private.h"
#include "source_private.h"
#include "private.h"

#ifndef DISPATCH_NO_LEGACY
#include "legacy.h"
#endif
/* More #includes at EOF (dependent on the contents of internal.h) ... */

uint64_t _dispatch_get_nanoseconds(void);

#ifndef DISPATCH_NO_LEGACY
dispatch_source_t
_dispatch_source_create2(dispatch_source_t ds,
	dispatch_source_attr_t attr,
	void *context,
	dispatch_source_handler_function_t handler);
#endif

void _dispatch_run_timers(void);
// Returns howsoon with updated time value, or NULL if no timers active.
struct timespec *_dispatch_get_next_timer_fire(struct timespec *howsoon);

dispatch_semaphore_t _dispatch_get_thread_semaphore(void);
void _dispatch_put_thread_semaphore(dispatch_semaphore_t);

bool _dispatch_source_testcancel(dispatch_source_t);

uint64_t _dispatch_timeout(dispatch_time_t when);
#if USE_POSIX_SEM
struct timespec _dispatch_timeout_ts(dispatch_time_t when);
#endif

extern bool _dispatch_safe_fork;

struct _dispatch_hw_config_s {
	uint32_t cc_max_active;
	uint32_t cc_max_logical;
	uint32_t cc_max_physical;
};
extern struct _dispatch_hw_config_s _dispatch_hw_config;

// some internally used funtions
uint64_t _dispatch_timeout(dispatch_time_t when);
struct timespec _dispatch_timeout_ts(dispatch_time_t when);
uint64_t _dispatch_get_nanoseconds(void);
bool _dispatch_source_testcancel(dispatch_source_t);
void libdispatch_init(void);
long dummy_function_r0();
void dummy_function();
void _dispatch_queue_cleanup(void *ctxt);

#endif /* DISPATCH_INTERNAL_H_ */
