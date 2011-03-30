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
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <pthread_workqueue.h>

#ifndef DEBUG
#	ifndef NDEBUG
#		define NDEBUG 
#	endif
#else
# define DISPATCH_DEBUG
#endif
#include <assert.h>

#ifdef _WIN32
# define DISPATCH_EXPORT __declspec(dllexport)
#else
# define DISPATCH_EXPORT __attribute__((visibility("default")))
#endif

#ifdef _WIN32
# include "shim/windows.h"
#else
# include "shim/posix.h"
#endif
#include "shim/atomic.h"

#ifdef __BLOCKS__
# include <Block.h>
# include "shim/Block_private.h"

#endif

#ifndef TRUE
#	define TRUE 1
#endif

#ifndef FALSE
#	define FALSE 0
#endif

/* I wish we had __builtin_expect_range() */
#if __GNUC__
#define fastpath(x)	((typeof(x))__builtin_expect((long)(x), ~0l))
#define slowpath(x)	((typeof(x))__builtin_expect((long)(x), 0l))
#else
#define fastpath(x) (x)
#define slowpath(x) (x)
#endif

#define DISPATCH_API_VERSION 20090501

#ifndef __DISPATCH_INDIRECT__
#define __DISPATCH_INDIRECT__
#endif

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

#   include "debug.h"
#	include "config.h"
#	include "datatypes.h"
#	include "taskqueue.h"
#	include "execution.h"
#	include "events.h"
#   include "threadmanager.h"

// the global queues
extern dispatch_queue_t _dispatch_global_q[];
void _dispatch_root_queues_init();

// some internally used funtions
void _dispatch_async_fast_exists_f(dispatch_queue_t queue, _taskitem_t i);
void _dispatch_async_fast_f(dispatch_queue_t queue, void *context, dispatch_function_t work);
struct timespec _dispatch_time_to_spec(dispatch_time_t t);
dispatch_time_t _dispatch_spec_to_time(const struct timespec* s);

#ifdef __BLOCKS__
dispatch_block_t _dispatch_Block_copy(dispatch_block_t db);

void _dispatch_call_block_and_release(void *block);
void _dispatch_call_block_and_release2(void *block, void *ctxt);
#endif
#undef __DISPATCH_INDIRECT__

#endif /* QUEUE_INTERNAL_H_ */
