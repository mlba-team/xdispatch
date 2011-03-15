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
		dispatch_async(dispatch_queue_t queue, dispatch_block_t block){
#ifndef __BLOCKS__
	dispatch_async_f(queue, (void*)new dispatch_block_t(block), runBlock);
#else
	dispatch_async_f(queue, (void *)block, runBlock);
#endif
}

void
		dispatch_sync(dispatch_queue_t queue, dispatch_block_t block){
#ifndef __BLOCKS__
	dispatch_sync_f(queue, (void*)new dispatch_block_t(block), runBlock);
#else
	dispatch_sync_f(queue, (void *)block, runBlock);
#endif
}

void
		dispatch_apply(size_t iterations, dispatch_queue_t queue, dispatch_iteration_block_t block){
#ifndef __BLOCKS__
	dispatch_apply_f(iterations, queue, (void*)new _block_function_s(block,iterations), runIterationBlock);
#else
	dispatch_apply_f(iterations, queue, (void *)block, runIterationBlock);
#endif
}

void
		dispatch_after(dispatch_time_t when, dispatch_queue_t queue, dispatch_block_t block){
#ifndef __BLOCKS__
	dispatch_after_f(when, queue, (void*)new dispatch_block_t(block), runBlock);
#else
	dispatch_after_f(when, queue, (void *)block, runBlock);
#endif
}

#endif
#endif
