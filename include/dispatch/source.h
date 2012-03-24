/*
 * Copyright (c) 2008-2009 Apple Inc. All rights reserved.
 *
 * @APPLE_APACHE_LICENSE_HEADER_START@
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
 * @APPLE_APACHE_LICENSE_HEADER_END@
 */

#ifndef __DISPATCH_SOURCE__
#define __DISPATCH_SOURCE__

/**
 * @addtogroup dispatch
 * @{
 */

#ifndef __DISPATCH_INDIRECT__
#error "Please #include <dispatch/dispatch.h> instead of this file directly."
#include "base.h"  // for HeaderDoc
#endif

/**
 * The dispatch framework provides a suite of interfaces for monitoring low-
 * level system objects (file descriptors, Mach ports, signals, VFS nodes, etc.)
 * for activity and automatically submitting event handler blocks to dispatch
 * queues when such activity occurs.
 *
 * This suite of interfaces is known as the Dispatch Source API.
 */

/**
 * Dispatch sources are used to automatically submit event handler blocks to
 * dispatch queues in response to external events.
 */
DISPATCH_DECL(dispatch_source);

/**
 * Constants of this type represent the class of low-level system object that
 * is being monitored by the dispatch source. Constants of this type are
 * passed as a parameter to dispatch_source_create() and determine how the
 * handle argument is interpreted (i.e. as a file descriptor, mach port,
 * signal number, process identifer, etc.), and how the mask arugment is
 * interpreted.
 */
typedef const struct dispatch_source_type_s *dispatch_source_type_t;

__DISPATCH_BEGIN_DECLS

/**
 * A dispatch source that coalesces data obtained via calls to
 * dispatch_source_merge_data(). An ADD is used to coalesce the data.
 * The handle is unused (pass zero for now).
 * The mask is unused (pass zero for now).
 */
#define DISPATCH_SOURCE_TYPE_DATA_ADD (&_dispatch_source_type_data_add)

extern DISPATCH_EXPORT
const struct dispatch_source_type_s _dispatch_source_type_data_add;

/**
 * A dispatch source that coalesces data obtained via calls to
 * dispatch_source_merge_data(). A logical OR is used to coalesce the data.
 * The handle is unused (pass zero for now).
 * The mask is used to perform a logical AND with the value passed to
 * dispatch_source_merge_data().
 */
#define DISPATCH_SOURCE_TYPE_DATA_OR (&_dispatch_source_type_data_or)

extern DISPATCH_EXPORT
const struct dispatch_source_type_s _dispatch_source_type_data_or;

/**
 * A dispatch source that monitors a Mach port for dead name
 * notifications (send right no longer has any corresponding receive right).
 * The handle is a Mach port with a send or send-once right (mach_port_t).
 * The mask is a mask of desired events from dispatch_source_mach_send_flags_t.
 */
#define DISPATCH_SOURCE_TYPE_MACH_SEND (&_dispatch_source_type_mach_send)

extern DISPATCH_EXPORT
const struct dispatch_source_type_s _dispatch_source_type_mach_send;

/**
 * A dispatch source that monitors a Mach port for pending messages.
 * The handle is a Mach port with a receive right (mach_port_t).
 * The mask is unused (pass zero for now).
 */
#define DISPATCH_SOURCE_TYPE_MACH_RECV (&_dispatch_source_type_mach_recv)

extern DISPATCH_EXPORT
const struct dispatch_source_type_s _dispatch_source_type_mach_recv;

/**
 * A dispatch source that monitors an external process for events
 * defined by dispatch_source_proc_flags_t.
 * The handle is a process identifier (pid_t).
 * The mask is a mask of desired events from dispatch_source_proc_flags_t.
 */
#define DISPATCH_SOURCE_TYPE_PROC (&_dispatch_source_type_proc)

extern DISPATCH_EXPORT
const struct dispatch_source_type_s _dispatch_source_type_proc;

/**
 * A dispatch source that monitors a file descriptor for pending
 * bytes available to be read.
 * The handle is a file descriptor (int).
 * The mask is unused (pass zero for now).
 */
#define DISPATCH_SOURCE_TYPE_READ (&_dispatch_source_type_read)

extern DISPATCH_EXPORT
const struct dispatch_source_type_s _dispatch_source_type_read;

/**
 * A dispatch source that monitors the current process for signals.
 * The handle is a signal number (int).
 * The mask is unused (pass zero for now).
 */
#define DISPATCH_SOURCE_TYPE_SIGNAL (&_dispatch_source_type_signal)

extern DISPATCH_EXPORT
const struct dispatch_source_type_s _dispatch_source_type_signal;

/**
 * A dispatch source that submits the event handler block based
 * on a timer.
 * The handle is unused (pass zero for now).
 * The mask is unused (pass zero for now).
 */
#define DISPATCH_SOURCE_TYPE_TIMER (&_dispatch_source_type_timer)

extern DISPATCH_EXPORT
const struct dispatch_source_type_s _dispatch_source_type_timer;

/**
 * A dispatch source that monitors a file descriptor for events
 * defined by dispatch_source_vnode_flags_t.
 * The handle is a file descriptor (int).
 * The mask is a mask of desired events from dispatch_source_vnode_flags_t.
 */
#define DISPATCH_SOURCE_TYPE_VNODE (&_dispatch_source_type_vnode)

extern DISPATCH_EXPORT
const struct dispatch_source_type_s _dispatch_source_type_vnode;

/**
 * A dispatch source that monitors a file descriptor for available
 * buffer space to write bytes.
 * The handle is a file descriptor (int).
 * The mask is unused (pass zero for now).
 */
#define DISPATCH_SOURCE_TYPE_WRITE (&_dispatch_source_type_write)

extern DISPATCH_EXPORT 
const struct dispatch_source_type_s _dispatch_source_type_write;

__DISPATCH_END_DECLS

/**
 *
 */
enum {
    DISPATCH_MACH_SEND_DEAD = 0x1, /**< The receive right corresponding to the given send right was destroyed. */
};

/**
 *
 */
enum {
    DISPATCH_PROC_EXIT = 0x80000000, /**< The process has exited (perhaps cleanly, perhaps not). */
    DISPATCH_PROC_FORK = 0x40000000, /**< The process has created one or more child processes. */
    DISPATCH_PROC_EXEC = 0x20000000, /**< The process has become another executable image via exec*() or posix_spawn*(). */
    DISPATCH_PROC_SIGNAL = 0x08000000, /**< A Unix signal was delivered to the process. */
};

/**
 *
 */
enum {
    DISPATCH_VNODE_DELETE = 0x1, /**< The filesystem object was deleted from the namespace. */
    DISPATCH_VNODE_WRITE = 0x2, /**< The filesystem object data changed. */
    DISPATCH_VNODE_EXTEND = 0x4, /**< The filesystem object changed in size. */
    DISPATCH_VNODE_ATTRIB = 0x8, /**< The filesystem object metadata changed. */
    DISPATCH_VNODE_LINK	= 0x10, /**< The filesystem object link count changed. */
    DISPATCH_VNODE_RENAME = 0x20, /**< The filesystem object was renamed in the namespace. */
    DISPATCH_VNODE_REVOKE = 0x40, /**< The filesystem object was revoked. */
};

__DISPATCH_BEGIN_DECLS

/**
 * Creates a new dispatch source to monitor low-level system objects and auto-
 * matically submit a handler block to a dispatch queue in response to events.
 *
 * Dispatch sources are not reentrant. Any events received while the dispatch
 * source is suspended or while the event handler block is currently executing
 * will be coalesced and delivered after the dispatch source is resumed or the
 * event handler block has returned.
 *
 * Dispatch sources are created in a suspended state. After creating the
 * source and setting any desired attributes (i.e. the handler, context, etc.),
 * a call must be made to dispatch_resume() in order to begin event delivery.
 *
 * @param type
 * Declares the type of the dispatch source. Must be one of the defined
 * dispatch_source_type_t constants.
 * @param handle
 * The underlying system handle to monitor. The interpretation of this argument
 * is determined by the constant provided in the type parameter.
 * @param mask
 * A mask of flags specifying which events are desired. The interpretation of
 * this argument is determined by the constant provided in the type parameter.
 * @param queue
 * The dispatch queue to which the event handler block will be submited.
 */

DISPATCH_EXPORT  
dispatch_source_t
dispatch_source_create(dispatch_source_type_t type,
	uintptr_t handle,
	unsigned long mask,
	dispatch_queue_t queue);

/**
 * Sets the event handler block for the given dispatch source.
 *
 * @param source
 * The dispatch source to modify.
 * The result of passing NULL in this parameter is undefined.
 * 
 * @param handler
 * The event handler block to submit to the source's target queue.
 */
#ifdef __BLOCKS__

DISPATCH_EXPORT  
void
dispatch_source_set_event_handler(dispatch_source_t source,
	dispatch_block_t handler);
#endif /* __BLOCKS__ */

/**
 * Sets the event handler function for the given dispatch source.
 *
 * @param source
 * The dispatch source to modify.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param handler
 * The event handler function to submit to the source's target queue.
 * The context parameter passed to the event handler function is the current
 * context of the dispatch source at the time the handler call is made.
 * The result of passing NULL in this parameter is undefined.
 */

DISPATCH_EXPORT  
void
dispatch_source_set_event_handler_f(dispatch_source_t source,
	dispatch_function_t handler);

/**
 * Sets the cancellation handler block for the given dispatch source.
 *
 * The cancellation handler (if specified) will be submitted to the source's
 * target queue in response to a call to dispatch_source_cancel() once the
 * system has released all references to the source's underlying handle and
 * the source's event handler block has returned.
 *
 * @remarks
 * A cancellation handler is required for file descriptor and mach port based
 * sources in order to safely close the descriptor or destroy the port. Closing
 * the descriptor or port before the cancellation handler may result in a race
 * condition. If a new descriptor is allocated with the same value as the
 * recently closed descriptor while the source's event handler is still running,
 * the event handler may read/write data to the wrong descriptor.
 *
 * @param source
 * The dispatch source to modify.
 * The result of passing NULL in this parameter is undefined.
 * 
 * @param cancel_handler
 * The cancellation handler block to submit to the source's target queue.
 */
#ifdef __BLOCKS__

DISPATCH_EXPORT  
void
dispatch_source_set_cancel_handler(dispatch_source_t source,
	dispatch_block_t cancel_handler);
#endif /* __BLOCKS__ */

/**
 * Sets the cancellation handler function for the given dispatch source.
 *
 * See dispatch_source_set_cancel_handler() for more details.
 *
 * @param source
 * The dispatch source to modify.
 * The result of passing NULL in this parameter is undefined.
 *
 * @param cancel_handler
 * The cancellation handler function to submit to the source's target queue.
 * The context parameter passed to the event handler function is the current
 * context of the dispatch source at the time the handler call is made.
 */

DISPATCH_EXPORT  
void
dispatch_source_set_cancel_handler_f(dispatch_source_t source,
	dispatch_function_t cancel_handler);

/**
 * Asynchronously cancel the dispatch source, preventing any further invocation
 * of its event handler block.
 *
 * Cancellation prevents any further invocation of the event handler block for
 * the specified dispatch source, but does not interrupt an event handler
 * block that is already in progress.
 *
 * The cancellation handler is submitted to the source's target queue once the
 * the source's event handler has finished, indicating it is now safe to close
 * the source's handle (i.e. file descriptor or mach port).
 *
 * See dispatch_source_set_cancel_handler() for more information.
 *
 * @param source
 * The dispatch source to be canceled.
 * The result of passing NULL in this parameter is undefined.
 */

DISPATCH_EXPORT  
void
dispatch_source_cancel(dispatch_source_t source);

/**
 * Tests whether the given dispatch source has been canceled.
 *
 * @param source
 * The dispatch source to be tested.
 * The result of passing NULL in this parameter is undefined.
 *
 * @return
 * Non-zero if canceled and zero if not canceled.
 */

DISPATCH_EXPORT  
long
dispatch_source_testcancel(dispatch_source_t source);

/**
 * Returns the underlying system handle associated with this dispatch source.
 *
 * @param source
 * The result of passing NULL in this parameter is undefined.
 *
 * @return
 * The return value should be interpreted according to the type of the dispatch
 * source, and may be one of the following handles:
 *
 *  DISPATCH_SOURCE_TYPE_DATA_ADD:        n/a
 *  DISPATCH_SOURCE_TYPE_DATA_OR:         n/a
 *  DISPATCH_SOURCE_TYPE_MACH_SEND:       mach port (mach_port_t)
 *  DISPATCH_SOURCE_TYPE_MACH_RECV:       mach port (mach_port_t)
 *  DISPATCH_SOURCE_TYPE_PROC:            process identifier (pid_t)
 *  DISPATCH_SOURCE_TYPE_READ:            file descriptor (int)
 *  DISPATCH_SOURCE_TYPE_SIGNAL:          signal number (int) 
 *  DISPATCH_SOURCE_TYPE_TIMER:           n/a
 *  DISPATCH_SOURCE_TYPE_VNODE:           file descriptor (int)
 *  DISPATCH_SOURCE_TYPE_WRITE:           file descriptor (int)
 */

DISPATCH_EXPORT    
uintptr_t
dispatch_source_get_handle(dispatch_source_t source);

/**
 * Returns the mask of events monitored by the dispatch source.
 *
 * @param source
 * The result of passing NULL in this parameter is undefined.
 *
 * @return
 * The return value should be interpreted according to the type of the dispatch
 * source, and may be one of the following flag sets:
 *
 *  DISPATCH_SOURCE_TYPE_DATA_ADD:        n/a
 *  DISPATCH_SOURCE_TYPE_DATA_OR:         n/a
 *  DISPATCH_SOURCE_TYPE_MACH_SEND:       dispatch_source_mach_send_flags_t
 *  DISPATCH_SOURCE_TYPE_MACH_RECV:       n/a
 *  DISPATCH_SOURCE_TYPE_PROC:            dispatch_source_proc_flags_t
 *  DISPATCH_SOURCE_TYPE_READ:            n/a
 *  DISPATCH_SOURCE_TYPE_SIGNAL:          n/a
 *  DISPATCH_SOURCE_TYPE_TIMER:           n/a
 *  DISPATCH_SOURCE_TYPE_VNODE:           dispatch_source_vnode_flags_t
 *  DISPATCH_SOURCE_TYPE_WRITE:           n/a
 */

DISPATCH_EXPORT    
unsigned long
dispatch_source_get_mask(dispatch_source_t source);

/**
 * Returns pending data for the dispatch source.
 *
 * This function is intended to be called from within the event handler block.
 * The result of calling this function outside of the event handler callback is
 * undefined.
 *
 * @param source
 * The result of passing NULL in this parameter is undefined.
 *
 * @return
 * The return value should be interpreted according to the type of the dispatch
 * source, and may be one of the following:
 *
 *  DISPATCH_SOURCE_TYPE_DATA_ADD:        application defined data
 *  DISPATCH_SOURCE_TYPE_DATA_OR:         application defined data
 *  DISPATCH_SOURCE_TYPE_MACH_SEND:       dispatch_source_mach_send_flags_t
 *  DISPATCH_SOURCE_TYPE_MACH_RECV:       n/a
 *  DISPATCH_SOURCE_TYPE_PROC:            dispatch_source_proc_flags_t
 *  DISPATCH_SOURCE_TYPE_READ:            estimated bytes available to read
 *  DISPATCH_SOURCE_TYPE_SIGNAL:          number of signals delivered since
 *                                            the last handler invocation
 *  DISPATCH_SOURCE_TYPE_TIMER:           number of times the timer has fired
 *                                            since the last handler invocation
 *  DISPATCH_SOURCE_TYPE_VNODE:           dispatch_source_vnode_flags_t
 *  DISPATCH_SOURCE_TYPE_WRITE:           estimated buffer space available
 */

DISPATCH_EXPORT    
unsigned long
dispatch_source_get_data(dispatch_source_t source);

/**
 * Merges data into a dispatch source of type DISPATCH_SOURCE_TYPE_DATA_ADD or
 * DISPATCH_SOURCE_TYPE_DATA_OR and submits its event handler block to its
 * target queue.
 *
 * @param source
 * The result of passing NULL in this parameter is undefined.
 *
 * @param value
 * The value to coalesce with the pending data using a logical OR or an ADD
 * as specified by the dispatch source type. A value of zero has no effect
 * and will not result in the submission of the event handler block.
 */

DISPATCH_EXPORT  
void
dispatch_source_merge_data(dispatch_source_t source, unsigned long value);

/**
 * Sets a start time, interval, and leeway value for a timer source.
 *
 * Calling this function has no effect if the timer source has already been
 * canceled.
 * 
 * The start time argument also determines which clock will be used for the
 * timer. If the start time is DISPATCH_TIME_NOW or created with
 * dispatch_time() then the timer is based on mach_absolute_time(). Otherwise,
 * if the start time of the timer is created with dispatch_walltime() then the
 * timer is based on gettimeofday(3).
 * 
 * @param start
 * The start time of the timer. See dispatch_time() and dispatch_walltime()
 * for more information.
 *
 * @param interval
 * The nanosecond interval for the timer.
 *
 * @param leeway
 * A hint given to the system by the application for the amount of leeway, in
 * nanoseconds, that the system may defer the timer in order to align with other
 * system activity for improved system performance or power consumption. (For
 * example, an application might perform a periodic task every 5 minutes, with
 * a leeway of up to 30 seconds.)  Note that some latency is to be expected for
 * all timers even when a leeway value of zero is specified.
 */

DISPATCH_EXPORT  
void
dispatch_source_set_timer(dispatch_source_t source,
	dispatch_time_t start,
	uint64_t interval,
	uint64_t leeway);

__DISPATCH_END_DECLS

/** @} */

#endif
