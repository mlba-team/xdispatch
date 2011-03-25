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

#include "xdispatch_internal.h"

#if defined(XDISPATCH_HAS_BLOCKS) && !defined(__BLOCKS__)

void dispatch_group_async(dispatch_group_t group, dispatch_queue_t queue, dispatch_block_t block){
    dispatch_group_async_f(group,queue, (void*)new dispatch_block_t(block), run_block);
}

void dispatch_group_notify(dispatch_group_t group, dispatch_queue_t queue, dispatch_block_t block){
    dispatch_group_notify_f(group,queue, (void*)new dispatch_block_t(block), run_block);
}

void dispatch_async(dispatch_queue_t queue, dispatch_block_t block){
    dispatch_async_f(queue, (void*)new dispatch_block_t(block), run_block);
}

void dispatch_sync(dispatch_queue_t queue, dispatch_block_t block){
    dispatch_sync_f(queue, (void*)new dispatch_block_t(block), run_block);
}

void dispatch_apply(size_t iterations, dispatch_queue_t queue, dispatch_iteration_block_t block){
    dispatch_apply_f(iterations, queue, (void*)new _block_function_s(block,iterations), run_iteration_block);
}

void dispatch_after(dispatch_time_t when, dispatch_queue_t queue, dispatch_block_t block){
    dispatch_after_f(when, queue, (void*)new dispatch_block_t(block), run_block);
}

#endif
