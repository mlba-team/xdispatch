/*
* Copyright (c) 2008-2009 Apple Inc. All rights reserved.
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


/* When building on 10.6 with gcc 4.5.1 we can bypass
	Apple's blocks implementation in C++ as we have lambdas.
	This prevents a lot of errors from occuring
	*/

#ifndef XDISPATCH_BLOCKS_H_
#define XDISPATCH_BLOCKS_H_

#if defined(__cplusplus)
#define __DISPATCH_BEGIN_DECLS	extern "C" {
#define __DISPATCH_END_DECLS	}
#else
#define __DISPATCH_BEGIN_DECLS
#define __DISPATCH_END_DECLS
#endif

#include <stddef.h>
#include <stdint.h>
#ifndef WIN32
#	include <stdbool.h>
#endif
#include <stdarg.h>

#define DISPATCH_API_VERSION 20090501

#ifndef __DISPATCH_INDIRECT__
#define __DISPATCH_INDIRECT__
#endif

#include "group_blocks.h"
#include "queue_blocks.h"

#undef __DISPATCH_INDIRECT__

#endif /* XDISPATCH_BLOCKS_H_ */
