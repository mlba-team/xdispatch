/*
* queue.h
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


#ifndef XDISPATCH_QUEUE_H_
#define XDISPATCH_QUEUE_H_

/**
 * @addtogroup xdispatch
 * @{
 */

#ifndef __XDISPATCH_INDIRECT__
 # error "Please #include <xdispatch/dispatch.h> instead of this file directly."
#endif

#include <iostream>
#include <string>

__XDISPATCH_BEGIN_NAMESPACE

/**
Provides an interface for representing
a dispatch queue and methods that can be
called to modify or use the queue.

Read Apple's documentation of libDispatch
to understand the concept of tasks and
queues.

@see xdispatch::dispatch for creation of queues
*/
class XDISPATCH_EXPORT queue
    : public object
{
public:
    /**
      Creates a new queue using the given
      dispatch_queue_t object
      */
    queue (
        dispatch_queue_t
    );
    /**
      Creates a new serial queue featuring
      the given label
      */
    queue (
        const char *label
    );
    queue (
        const std::string &
    );
    queue (
        const queue &
    );
    ~queue ();

    /**
      Will dispatch the given operation for
      async execution on the queue and return
      immediately.

      The operation will be deleted as soon
      as it was executed. To change this behaviour,
      set the auto_delete flag of the operation.
      @see operation::auto_delete()
      */
    void async(
        operation *
    );

    /**
    Same as async(operation*).
    Will put the given function on the queue.

    @see async(operation*)
    */
    template< typename _Func >
    inline void async(
        const _Func &b
    )
    {
        async( ::xdispatch::make_operation( b ) );
    }

    /**
    Applies the given iteration_operation for async execution
    in this queue and returns immediately.

      The operation will be deleted as soon
      as it was executed the requested number of times.
      To change this behaviour, set the auto_delete flag
      of the operation.
      @see operation::auto_delete()

    @param times The number of times the operation will be executed
    */
    void apply(
        size_t times,
        iteration_operation *
    );

    /**
     * @deprecated Use the version with swapped arguments instead
     */
    XDISPATCH_DEPRECATED( inline void
        apply( iteration_operation * op, size_t times )
    )
    {
        apply( times, op );
    }

    /**
    Same as apply(iteration_operation*, size_t times).

    Will wrap the given function in an operation and put it on the
    queue.

    @see apply(size_t times, iteration_operation*)
    */
    template< typename _Func >
    inline void apply(
        size_t times,
        const _Func &b
    )
    {
        apply( times, ::xdispatch::make_iteration_operation( b ) );
    }

    /**
     * @deprecated Use the version with swapped arguments instead
     */
    template< typename _Func >
    XDISPATCH_DEPRECATED( inline void
        apply(
            const _Func &b,
            size_t times
        )
    )
    {
        apply< _Func > ( times, b );
    }

    /**
    Applies the given operation for async execution
    in this queue after the given time and returns immediately.
    The queue will take possession of the
    operation and handle the deletion. To change this behaviour,
    set the auto_delete flag of the operation.
    @see operation::auto_delete();

    @param time The time to wait until the operation is applied to
    the queue.
    */
    void after(
        struct tm *time,
        operation *
    );

    /**
    Applies the given operation for async execution
    in this queue after the given time and returns immediately.
    The queue will take possession of the
    operation and handle the deletion. To change this behaviour,
    set the auto_delete flag of the operation.
    @see operation::auto_delete();

    @param time The time to wait until the operation is applied to
    the queue.
    */
    void after(
        dispatch_time_t time,
        operation *
    );

    /**
     * @deprecated Use the version with swapped arguments instead
     */
    XDISPATCH_DEPRECATED( inline void
        after(
            operation * op,
            struct tm *time
        )
    )
    {
        after( time, op );
    }

    /**
     * @deprecated Use the version with swapped arguments instead
     */
    XDISPATCH_DEPRECATED( inline void
        after(
            operation * op,
            dispatch_time_t time
        )
    )
    {
        after( time, op );
    }

    /**
    Same as dispatch_after(operation*, time_t).
    Will wrap the given function in an operation and put it on the
    queue.
    */
    template< typename _Func >
    inline void after(
        struct tm *time,
        const _Func &b
    )
    {
        after( time, ::xdispatch::make_operation( b ) );
    }

    /**
    Same as dispatch_after(operation*, time_t).
    Will wrap the given block in an operation and put it on the
    queue.
    */
    template< typename _Func >
    inline void after(
        dispatch_time_t time,
        const _Func &b
    )
    {
        after( time, ::xdispatch::make_operation( b ) );
    }

    /**
     * @deprecated Use the version with swapped arguments instead
     */
    template< typename _Func >
    XDISPATCH_DEPRECATED( inline void
        after(
            const _Func &b,
            struct tm *time
        )
    )
    {
        after( b, time );
    }

    /**
     * @deprecated Use the version with swapped arguments instead
     */
    template< typename _Func >
    XDISPATCH_DEPRECATED( inline void
        after(
            const _Func &b,
            dispatch_time_t time
        )
    )
    {
        after( b, time );
    }

    /**
    Applies the given operation for execution
    in this queue and blocks until the operation
    was executed. The queue will take possession of the
    operation and handle the deletion. To change this behaviour,
    set the auto_delete flag of the operation.
    @see operation::auto_delete();
    */
    void sync(
        operation *
    );

    /**
    Same as dispatch_sync(operation*).
    Will wrap the given function in an operation and put it on the
    queue.
    */
    template< typename _Func >
    inline void sync(
        const _Func &b
    )
    {
        sync( ::xdispatch::make_operation( b ) );
    }

    /**
    Sets the given operation as finalizer for this
    queue. A finalizer is called before destroying
    a queue, i.e. if all queue objects
    representing the queue were deleted and all
    pending work on a queue was dispatched. The queue will take possession of the
    operation and handle the deletion. To change this behaviour,
    set the auto_delete flag of the operation.
    @see operation::auto_delete();

    When not passing a queue, the finalizer operation
    will be executed on the queue itself.
    */
    void finalizer(
        operation *,
        const queue & = global_queue()
    );

    /**
    Same as set_finalizer(operation*, queue*).
    Will wrap the given function in an operation and store
    it as finalizer.
    */
    template< typename _Func >
    inline void finalizer(
        const _Func &b,
        const queue &q = global_queue()
    )
    {
        finalizer( ::xdispatch::make_operation( b ), q );
    }

    /**
    @return The label of the queue that was used while creating it
    */
    const std::string & label() const;

    /**
    @returns The dispatch_queue_t object associated with this
    C++ object. Use this, if you need to use the plain C Interface
    of libdispatch.
    @see native_queue()
    */
    virtual dispatch_object_t native() const;

    /**
    @returns The dispatch_queue_t object associated with this
    C++ object. Use this, if you need to use the plain C Interface
    of libdispatch.
    @see native()
    */
    virtual dispatch_queue_t native_queue() const;

    queue & operator = (
        const queue &
    );


private:
    class data;
    pointer< data >::unique d;
};


XDISPATCH_EXPORT std::ostream &
operator << (
    std::ostream &,
    const queue *
);

XDISPATCH_EXPORT std::ostream &
operator << (
    std::ostream &,
    const queue &
);

__XDISPATCH_END_NAMESPACE

/** @} */

#endif /* XDISPATCH_QUEUE_H_ */