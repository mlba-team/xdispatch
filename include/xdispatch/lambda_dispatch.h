/*
* lambda_dispatch.h
*
* Copyright (c) 2008-2009 Apple Inc.
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


/* When building on 10.6 with gcc 4.5.1 we can bypass
    Apple's lambda functions implementation in C++ as we have lambdas.
  This prevents a lot of errors from occuring
  */

#ifndef XDISPATCH_LAMBDA_DISPATCH_H_
#define XDISPATCH_LAMBDA_DISPATCH_H_

/**
 * @addtogroup xdispatch
 * @{
 */

#ifndef __XDISPATCH_INDIRECT__
 # error "Please #include <xdispatch/dispatch.h> instead of this file directly."
 # include "dispatch.h"
#endif

/// provide block specific libdispatch functions as well

#if ( defined __cplusplus ) && !XDISPATCH_HAS_BLOCKS

// needed for dispatch_source_t compatibility
void XDISPATCH_EXPORT _xdispatch_source_set_event_handler( dispatch_source_t, xdispatch::operation * op );
void XDISPATCH_EXPORT _xdispatch_source_set_cancel_handler( dispatch_source_t, xdispatch::operation * op );

template< typename _Func >
inline void dispatch_async(
    dispatch_queue_t queue,
    const _Func &block
)
{
    xdispatch::queue( queue ).async( block );
}


template< typename _Func >
inline void dispatch_after(
    dispatch_time_t when,
    dispatch_queue_t queue,
    const _Func &block
)
{
    xdispatch::queue( queue ).after( when, block );
}


template< typename _Func >
inline void dispatch_sync(
    dispatch_queue_t queue,
    const _Func &block
)
{
    xdispatch::queue( queue ).sync( block );
}


template< typename _Func >
inline void dispatch_apply(
    size_t iterations,
    dispatch_queue_t queue,
    const _Func &block
)
{
    xdispatch::queue( queue ).apply( iterations, block );
}


template< typename _Func >
inline void dispatch_group_async(
    dispatch_group_t group,
    dispatch_queue_t queue,
    const _Func &block
)
{
    xdispatch::group( group ).async( block, queue );
}


template< typename _Func >
inline void dispatch_group_notify(
    dispatch_group_t group,
    dispatch_queue_t queue,
    const _Func &block
)
{
    xdispatch::group( group ).notify( block, queue );
}


template< typename _Func >
inline void dispatch_source_set_event_handler(
    dispatch_source_t source,
    const _Func &handler
)
{
    _xdispatch_source_set_event_handler( source, xdispatch::make_operation( handler ) );
}


template< typename _Func >
inline void dispatch_source_set_cancel_handler(
    dispatch_source_t source,
    const _Func &cancel_handler
)
{
    _xdispatch_source_set_cancel_handler( source, xdispatch::make_operation( cancel_handler ) );
}


 # ifdef dispatch_once
  #  undef dispatch_once
 # endif

template< typename _Func >
inline void dispatch_once(
    dispatch_once_t *predicate,
    const _Func &block
)
{
    xdispatch::once o( predicate );

    o( block );
}


#endif // if ( defined __cplusplus ) && !XDISPATCH_HAS_BLOCKS

/** @} */

#endif /* XDISPATCH_LAMBDA_DISPATCH_H_ */
