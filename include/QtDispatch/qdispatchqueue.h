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



#ifndef QDISPATCH_QUEUE
#define QDISPATCH_QUEUE

#include "../xdispatch/dispatch.h"

#include <memory>

#include "qdispatchglobal.h"
#include <QString>
#include <QDebug>
class QTime;
class QRunnable;
class QIterationRunnable;

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Dispatch)

/**
Provides an interface for representing
a dispatch queue and methods that can be
called to modify or use the queue.

Read Apple's documentation of libDispatch
to understand the concept of tasks and
queues.

@see QDispatch for creating QDispatchQueues
*/
class QDispatchQueue {

public:
    /**
     A pointer automatically deleting
     the QDispatchQueue when going out
     of scope.
     */
    typedef std::auto_ptr< QDispatchQueue> APtr;

	/**
	Applies the given QRunnable for async execution
	in this queue and returns immediately.
	*/
	virtual void dispatch(QRunnable*) = 0;
#ifdef XDISPATCH_HAS_BLOCKS
	/**
	Same as dispatch(QRunnable*).
	Will wrap the given block in a QRunnable and put it on the
	queue.
	*/
	virtual void dispatch(dispatch_block_t) = 0;
#endif
	/**
	Applies the given QRunnable for async execution
	in this queue and returns immediately.

    In case the autoDelete() flag of the passed QIterationRunnable
    is set to true it is ensured that the runnable will be deleted
    after being executed the requested number of times

	@param times The number of times the QRunnable will be executed
	*/
	virtual void dispatch(QIterationRunnable*, int times) = 0;
#ifdef XDISPATCH_HAS_BLOCKS
	/**
	Same as dispatch(QRunnable*,int times).

	Will wrap the given block in a QRunnable and put it on the
	queue.

    @see dispatch(QRunnable*, int times)
	*/
    virtual void dispatch(dispatch_iteration_block_t, int times) = 0;
#endif
	/**
	Applies the given QRunnable for async execution
	in this queue after the given time and returns immediately
	@param time The time to wait until the QRunnable is applied to
	the queue.
	*/
	virtual void dispatchAfter(QRunnable*, const QTime& time) = 0;
	virtual void dispatchAfter(QRunnable*, dispatch_time_t time) = 0;
#ifdef XDISPATCH_HAS_BLOCKS
	/**
	Same as dispatchAfter().
	Will wrap the given block in a QRunnable and put it on the
	queue.
	*/
    virtual void dispatchAfter(dispatch_block_t, const QTime& time) = 0;
    virtual void dispatchAfter(dispatch_block_t, dispatch_time_t time) = 0;
#endif
	/**
	Applies the given QRunnable for execution
	int his queue and blocks until the QRunnable
	was executed
	*/
	virtual void dispatchSync(QRunnable*) = 0;
#ifdef XDISPATCH_HAS_BLOCKS
	/**
	Same as dispatchSync(QRunnable*,int times).
	Will wrap the given block in a QRunnable and put it on the
	queue.
	*/
	virtual void dispatchSync(dispatch_block_t) = 0;
#endif
	/**
	Sets the given runnable as finalizer for this
	queue. A finalizer is called before destroying
	a queue, i.e. if all QDispatchQueue objects
	representing the queue were deleted and all
	pending work on a queue was dispatched.

	@remarks Finalizers will never be called on the
	global queues or the main queue.
	*/
	virtual void setFinalizer(QRunnable*, QDispatchQueue* = NULL) = 0;
#ifdef XDISPATCH_HAS_BLOCKS
	/**
	Same as dispatchSync(QRunnable*,int times).
	Will wrap the given block in a QRunnable and store
	it as finalizer.

	@remarks Finalizers will never be called on the
	global queues or the main queue.
	*/
	virtual void setFinalizer(dispatch_block_t, QDispatchQueue* = NULL) = 0;
#endif
	/**
	@return The label of the queue that was used while creating it
	*/
	virtual const QString label() const = 0;
	/**
        Suspends the invocation of work items on this queue.

        A suspended queue will not invoke any blocks associated with it. The
        suspension of a queue will occur after any running work item associated with
        the queue completes.

        Calls to suspend() must be balanced with calls to resume().
	*/
        virtual void suspend() = 0;
        /**
        Resumes the invocation of work items on this queue.

        Calls to suspend() must be balanced with calls to resume().
        */
        virtual void resume() = 0;
	/**
	@returns The dispatch_queue_t object associated with this
	C++ object (if any). Use this, if you need to use the plain C Interface
        of libDispatch/libXDispatch. Please not that in the mainqueue there
	will be no native queue and NULL will be returned.
	*/
	virtual const dispatch_queue_t nativeQueue() const { return NULL; }

};

static QDebug operator<<(QDebug dbg, const QDispatchQueue* q)
{
        dbg.nospace() << "DispatchQueue " << q->label();
	return dbg.space();
}
static QDebug operator<<(QDebug dbg, const QDispatchQueue::APtr q)
{
    dbg.nospace() << "DispatchQueue " << q->label();
	return dbg.space();
}

QT_END_NAMESPACE
QT_END_HEADER

#endif /* QDISPATCH_QUEUE */
