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



#ifndef QDISPATCH_H_
#define QDISPATCH_H_

#include "../dispatch.h"
#include "qdispatchglobal.h"
#include "qdispatchqueue.h"
#include "qsynchronized.h"

class QDispatchQueue;
class QTime;
class QString;

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Dispatch)

/**
A Shortcut for quickly accessing the
QDispatch instance of your program
*/
#define QD QDispatch::instance

/**
Single Instance Interface to control the dispatch behaviour.
Use this object to create new queues, get one of the main or
global queues.

When using this object from within a QRunnable executed on a
queue you can also use it to get the queue you're running in.

For information about the Dispatch mechanism, please
also see Apple's documentation on libDispatch.
*/
class QDispatch {

public:
    
	/**
	Three priority classes used for the three standard
	global queues
	*/
        enum Priority { HIGH, DEFAULT, LOW };

	/**
	Use this to access the QDispatch instance
	of your program
	*/
	static QDispatch* instance;
	/**
	Create a new serial queue with the given label.
	All items dispatched to this queue will be executed
	in FIFO order. Please note that still items in different
	queues can and will be executed concurrently.

	@return NULL if something went wrong
	*/
	QDispatchQueue* createQueue(const QString& label);
	/**
	Returns the main queue. This is the queue running
	within the Qt Event Loop. Thus only items put
	on this queue can change the GUI.
	@return NULL if something went wrong
	*/
	QDispatchQueue* getMainQueue();
	/**
	Returns the global queue associated to the given
	Priority p.

	Runnables submitted to these global concurrent queues
	may be executed concurrently with respect to
	each other.
	@return NULL if something went wrong
	*/
    QDispatchQueue* getGlobalQueue(Priority p = DEFAULT);
	/**
	@return The queue this runnable is executed in or NULL if
	this function is not called from a QRunnable executed
	in a queue.
     
    @remarks The value returned here is wrapped within
        QDispatchQueue::APtr which is a typedef for
        std::auto_ptr. This has the benefit of calling
        getCurrentQueue() without creating any leaks or a
        need to call delete. To control the lifetime of
        the QDispatchQueue object yourself, please use
        APtr.release() and have a look at the documentation
        of std::auto_ptr
    @see QDispatchQueue::APtr
	*/
    QDispatchQueue::APtr getCurrentQueue();
	/**
	@return The given QTime converted to a dispatch_time_t
	*/
	dispatch_time_t asDispatchTime(const QTime&);
	/**
	@remarks Please be careful when using this converter as
	a QTime is tracking 24 hours max, whereas a
	dispatch_time_t can hold way more. This additional
	time will be cropped while converting.

	@return The given dispatch_time_t as QTime
	*/
	QTime asQTime(dispatch_time_t t);

private:
	QDispatch();
	QDispatch(const QDispatch&);
	~QDispatch();

	class Private;
	Private* d;
};

QT_END_NAMESPACE
QT_END_HEADER

#endif /* QDISPATCH_H_ */
