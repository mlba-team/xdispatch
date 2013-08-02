/*
* dispatch.h
*
* Copyright (c) 2011-2013 MLBA-Team.
* All rights reserved.
*
* @LICENSE_HEADER_START@
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
* @LICENSE_HEADER_END@
*/


#ifndef XDISPATCH_H_
#define XDISPATCH_H_

/**
 * @addtogroup xdispatch
 * @{
 */

#include <dispatch/dispatch.h>
#ifndef NSEC_PER_MSEC
 # define NSEC_PER_MSEC 1000000ll
#endif

// quick hack to force the definition of these macros despite a native
// libdispatch implementation
#ifdef __APPLE__
 # define DISPATCH_SOURCE_HAS_DATA_ADD 1
 # define DISPATCH_SOURCE_HAS_DATA_OR 1
 # define DISPATCH_SOURCE_HAS_MACH_SEND 1
 # define DISPATCH_SOURCE_HAS_MACH_RECV 1
 # define DISPATCH_SOURCE_HAS_PROC 1
 # define DISPATCH_SOURCE_HAS_READ 1
 # define DISPATCH_SOURCE_HAS_SIGNAL 1
 # define DISPATCH_SOURCE_HAS_TIMER 1
 # define DISPATCH_SOURCE_HAS_VNODE 1
 # define DISPATCH_SOURCE_HAS_WRITE 1
#endif // ifdef __APPLE__

#include "platform.h"
#include "dispatch_decl.h"

#if defined ( __cplusplus )

 # define __XDISPATCH_INDIRECT__
 # include "pointer.h"
 # include "blocks.h"
 # include "base.h"
 # include "operation.h"
 # include "semaphore.h"
 # include "synchronized.h"
 # include "once.h"
 # include "queue.h"
 # include "group.h"
 # include "source.h"
 # include "timer.h"
 # include "lambda_dispatch.h"
 # undef __XDISPATCH_INDIRECT__

 # undef XDISPATCH_EXPORT

#endif /* defined(__cplusplus) */


/** @} */

#endif /* XDISPATCH_H_ */
