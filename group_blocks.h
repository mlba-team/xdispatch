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



#ifndef __DISPATCH_GROUP_BLOCKS__
#define __DISPATCH_GROUP_BLOCKS__

#ifndef __DISPATCH_INDIRECT__
#error "Please #include <xdispatch/dispatch.h> instead of this file directly."
#include "base.h" // for HeaderDoc
#endif

__DISPATCH_BEGIN_DECLS

#ifdef HAS_BLOCKS
void
dispatch_group_async(dispatch_group_t group,
	dispatch_queue_t queue,
	dispatch_block_t block);
#endif /* HAS_BLOCKS */

#ifdef HAS_BLOCKS
void
dispatch_group_notify(dispatch_group_t group,
	dispatch_queue_t queue,
	dispatch_block_t block);
#endif /* HAS_BLOCKS */


__DISPATCH_END_DECLS

#endif /* __DISPATCH_GROUP_BLOCKS__*/
