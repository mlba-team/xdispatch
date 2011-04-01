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

#include <map>

#include "xdispatch_internal.h"

#if defined(XDISPATCH_HAS_BLOCKS) && !defined(__BLOCKS__)

__XDISPATCH_USE_NAMESPACE

void dispatch_group_async(dispatch_group_t group, dispatch_queue_t queue, dispatch_block_t block){
    dispatch_group_async_f(group,queue, new wrap(block), run_wrap);
}

void dispatch_group_notify(dispatch_group_t group, dispatch_queue_t queue, dispatch_block_t block){
    dispatch_group_notify_f(group,queue, new wrap(block), run_wrap);
}

void dispatch_async(dispatch_queue_t queue, dispatch_block_t block){
    dispatch_async_f(queue, new wrap(block), run_wrap);
}

void dispatch_sync(dispatch_queue_t queue, dispatch_block_t block){
    dispatch_sync_f(queue, new wrap(block), run_wrap);
}

void dispatch_apply(size_t iterations, dispatch_queue_t queue, dispatch_iteration_block_t block){
    dispatch_apply_f(iterations, queue, new iteration_wrap(block,iterations), run_iter_wrap);
}

void dispatch_after(dispatch_time_t when, dispatch_queue_t queue, dispatch_block_t block){
    dispatch_after_f(when, queue, new wrap(block), run_wrap);
}

static dispatch_semaphore_t known_handler_lock = dispatch_semaphore_create(1);
static std::map<void*, dispatch_block_t> known_handlers;

static void dispatch_source_handler(void* ct){
    if(ct==NULL)
        return;

    dispatch_semaphore_wait(known_handler_lock, DISPATCH_TIME_FOREVER);
    dispatch_block_t work = known_handlers.at(ct);
    dispatch_semaphore_signal(known_handler_lock);
    work();
    XDISPATCH_BLOCK_RELEASE(work);
}

void dispatch_source_set_event_handler(dispatch_source_t source, dispatch_block_t handler){
    dispatch_block_t stored = XDISPATCH_BLOCK_COPY(handler);

    dispatch_semaphore_wait(known_handler_lock, DISPATCH_TIME_FOREVER);
    known_handlers.insert( std::pair<void*, dispatch_block_t>((void*)source,stored) );
    dispatch_semaphore_signal(known_handler_lock);


    // by using the context we can pass kind of a hash key to the handler
    // TODO: Make this safer, as someone else could call 'dispatch_set_context'
    dispatch_set_context(source, (void*)source);
    dispatch_source_set_event_handler_f(source, dispatch_source_handler);
}

static dispatch_semaphore_t known_cancel_handler_lock = dispatch_semaphore_create(1);
static std::map<void*, dispatch_block_t> known_cancel_handlers;

static void dispatch_source_cancel_handler(void* ct){
    if(ct==NULL)
        return;

    dispatch_semaphore_wait(known_cancel_handler_lock, DISPATCH_TIME_FOREVER);
    dispatch_block_t work = known_cancel_handlers.at(ct);
    dispatch_semaphore_signal(known_cancel_handler_lock);
    work();
    XDISPATCH_BLOCK_RELEASE(work);
}

void dispatch_source_set_cancel_handler(dispatch_source_t source, dispatch_block_t cancel_handler){
    dispatch_block_t stored = XDISPATCH_BLOCK_COPY(cancel_handler);

    dispatch_semaphore_wait(known_cancel_handler_lock, DISPATCH_TIME_FOREVER);
    known_cancel_handlers.insert( std::pair<void*, dispatch_block_t>((void*)source,stored) );
    dispatch_semaphore_signal(known_cancel_handler_lock);

    // by using the context we can pass kind of a hash key to the handler
    // TODO: Make this safer, as someone else could call 'dispatch_set_context'
    dispatch_set_context(source, (void*)source);
    dispatch_source_set_event_handler_f(source, dispatch_source_cancel_handler);
}

#endif
