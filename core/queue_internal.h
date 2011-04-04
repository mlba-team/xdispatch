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


#ifndef QUEUE_INTERNAL_H_
#define QUEUE_INTERNAL_H_

#if defined(__cplusplus)
#define __DISPATCH_BEGIN_DECLS	extern "C" {
#define __DISPATCH_END_DECLS	}
#else
#define __DISPATCH_BEGIN_DECLS
#define __DISPATCH_END_DECLS
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef DEBUG
#	ifndef NDEBUG
#		define NDEBUG 
#	endif
#endif
#include <assert.h>


#ifdef WIN32
#	include "os_windows.h"
#	include "threads_windows.h"
#else
#	include <sys/time.h>
#	include <unistd.h>
#	include <pthread.h>
#   include <semaphore.h>
#	include <stdbool.h>
#endif

#ifndef TRUE
#	define TRUE 1
#endif

#ifndef FALSE
#	define FALSE 0
#endif

// define grade of implementation
#ifdef HAVE_NATIVE_DISPATCH_H
#   if !defined(__clang__) && defined(HAS_BLOCKS)
#       define BLOCKS_IMPL_ONLY
#   endif
#else
#   if defined(__BLOCKS__)
#       define DISPATCH_IMPL_ONLY
#   else
#       define DISPATCH_FULL_IMPL
#	endif
#endif

#define DISPATCH_API_VERSION 20090501

// Apple specific hack
#ifdef HAVE_NATIVE_DISPATCH_H
#	include <dispatch/dispatch.h>
#endif

#ifndef __DISPATCH_INDIRECT__
#define __DISPATCH_INDIRECT__
#endif

// Apple specific hack
#ifdef BLOCKS_IMPL_ONLY 
#	include "../group_blocks.h"
#	include "../queue_blocks.h"
#elif !defined(HAVE_NATIVE_DISPATCH_H)

#	include "../include/libdispatch/base.h"
#	include "../include/libdispatch/object.h"
#	include "../include/libdispatch/time.h"
#	include "../include/libdispatch/queue.h"
/* DISABLED UNTIL CLEAN IMPLEMENTATION IS AVAILABLE
#	include "source.h"
   */
#	include "../include/libdispatch/group.h"
#	include "../include/libdispatch/semaphore.h"
#	include "../include/libdispatch/once.h"

__DISPATCH_BEGIN_DECLS
#	include "config.h"
#	include "atomic.h"
#	include "datatypes.h"
#	include "taskqueue.h"
#	include "threadmanager.h"
#	include "execution.h"
#	include "events.h"
#	include "threadpool.h"
__DISPATCH_END_DECLS

// the global queues
extern dispatch_queue_t _dispatch_global_q[];

// some internally used funtions
inline void _dispatch_async_fast_exists_f(dispatch_queue_t queue, _taskitem_t i);
inline void _dispatch_async_fast_f(dispatch_queue_t queue, void *context, dispatch_function_t work);
struct timespec _dispatch_time_to_spec(dispatch_time_t t);
dispatch_time_t _dispatch_spec_to_time(const struct timespec* s);

#endif

#undef __DISPATCH_INDIRECT__

#endif /* QUEUE_INTERNAL_H_ */
