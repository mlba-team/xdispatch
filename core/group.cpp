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



#include "queue_internal.h"
#include "runBlocks.h"

#ifdef HAS_BLOCKS
#if defined(BLOCKS_IMPL_ONLY) || defined(DISPATCH_FULL_IMPL)

void
dispatch_group_async(dispatch_group_t group, dispatch_queue_t queue, dispatch_block_t block){
#ifndef __BLOCKS__
	dispatch_group_async_f(group,queue, (void*)new dispatch_block_t(block), runBlock);
#else
	dispatch_group_async_f(group,queue, (void *)block, runBlock);
#endif
}


void
dispatch_group_notify(dispatch_group_t group, dispatch_queue_t queue, dispatch_block_t block){
#ifndef __BLOCKS__
	dispatch_group_notify_f(group,queue, (void*)new dispatch_block_t(block), runBlock);
#else
	dispatch_group_notify_f(group,queue, (void *)block, runBlock);
#endif
}

#endif
#endif
