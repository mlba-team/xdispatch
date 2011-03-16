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



#ifndef QDISPATCH_GROUP_H_
#define QDISPATCH_GROUP_H_

#include <QObject>
#include <QDebug>
#include "../dispatch.h"

class QRunnable;
class QDispatchQueue;
class QTime;

/**
A DispatchGroup is a group of QRunnables
dispatched on queues. This class provides
a way to track all these QRunnables
and notify you if all of them finished
executing.

See also Apple's Documentation of Dispatch Groups
*/

class QDispatchGroup : public QObject {

	Q_OBJECT

public:
	/**
	Creates a new QDispatchGroup.
	*/
	QDispatchGroup();
	QDispatchGroup(const QDispatchGroup&);
	~QDispatchGroup();

	/**
	Dispatches a Runnable on the given Queue
	@param r The QRunnable to be dispatched
	@param q The Queue to use. If no Queue is given, the system default queue will be used
	*/
	void dispatch(QRunnable* r, QDispatchQueue* q = NULL);
#ifdef HAS_BLOCKS
	/**
	Same as dispatch(QRunnable* r, ...)
	Will wrap the given block in a QRunnable and put it on the queue.
	*/
	void dispatch(dispatch_block_t b, QDispatchQueue* q = NULL);
#endif
	/**
	Waits until the given time has passed
	or all dispatched runnables in the group were executed
	@param t give a time here or a DISPATCH_TIME_FOREVER to wait until all runnables are done
	@return false if the timeout occured or true if all runnables were executed
	*/
	bool wait(dispatch_time_t = DISPATCH_TIME_FOREVER);
	/**
	Waits until the given time has passed
	or all dispatched runnables in the group were executed
	@param timeout give a timeout here or a QTime of zero to wait until all runnables are done
	@return false if the timeout occured or true if all runnables were executed
	*/
	bool wait(const QTime& t);
	/**
	This function schedules a notification runnable to be submitted to the specified
	queue once all runnables associated with the dispatch group have completed.

	If no runnables are associated with the dispatch group (i.e. the group is empty)
	then the notification runnable will be submitted immediately.

	The runnable will be empty at the time the notification block is submitted to
	the target queue. The group may either be deleted
	or reused for additional operations.
	@see dispatch() for more information.
	*/
	void notify(QRunnable* r, QDispatchQueue* q = NULL);
#ifdef HAS_BLOCKS
	/**
	This function schedules a notification block to be submitted to the specified
	queue once all blocks associated with the dispatch group have completed.

	If no blocks are associated with the dispatch group (i.e. the group is empty)
	then the notification block will be submitted immediately.

	The group will be empty at the time the notification block is submitted to
	the target queue. The group may either be deleted
	or reused for additional operations.
	@see dispatch() for more information.
	*/
	void notify(dispatch_block_t b, QDispatchQueue* q = NULL);
#endif

signals:
	/**
	This will be emitted additionally to any function
	or block submitted via notify().

	Every time the all work dispatched to the group (i.e.
	the group is empty) this signal will be emitted.
	*/
	void allFinished();

private:
    friend class _GroupRunnable;
	friend QDebug operator<<(QDebug, const QDispatchGroup&);
    
	class Private;
	Private* d;
};

QDebug operator<<(QDebug, const QDispatchGroup&);

#endif /* QDISPATCH_GROUP_H_ */
