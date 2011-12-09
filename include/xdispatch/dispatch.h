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


#ifndef XDISPATCH_H_
#define XDISPATCH_H_

# ifdef HAVE_NATIVE_DISPATCH_H
#  include <dispatch/dispatch.h>
#  ifndef NSEC_PER_MSEC
#   define NSEC_PER_MSEC 1000000ll
#  endif
# else
#  include "../dispatch/dispatch.h"
# endif

#if defined(__cplusplus)

#ifndef XDISPATCH_EXPORT
# ifdef _WIN32
#  ifdef XDISPATCH_MAKEDLL
#   define XDISPATCH_EXPORT __declspec(dllexport)
#  else
#   define XDISPATCH_EXPORT __declspec(dllimport)
#  endif
# else
#  define XDISPATCH_EXPORT __attribute__((visibility("default")))
# endif
#endif

# define __XDISPATCH_BEGIN_NAMESPACE	namespace xdispatch {
# define __XDISPATCH_END_NAMESPACE }

# define __XDISPATCH_INDIRECT__
# include "synchronized.h"
# include "lambda_blocks.h"
# include "base.h"
# include "once.h"
# include "queue.h"
# include "group.h"
# include "semaphore.h"
# include "source.h"
# include "timer.h"
# include "lambda_dispatch.h"
# undef __XDISPATCH_INDIRECT__

#undef XDISPATCH_EXPORT

#endif /* defined(__cplusplus) */

#endif /* XDISPATCH_H_ */
