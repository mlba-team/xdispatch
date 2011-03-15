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



#ifndef _DISPATCHQUEUEIMPLH_H_
#define _DISPATCHQUEUEIMPLT_H_

#include "../dispatch.h"
#include "QDispatchQueue.h"

class _DispatchQueueImpl : public QDispatchQueue {

public:
        _DispatchQueueImpl(dispatch_queue_t);
	_DispatchQueueImpl(const _DispatchQueueImpl&);
	~_DispatchQueueImpl();

	virtual void dispatch(QRunnable*);
	virtual void dispatch(QIterationRunnable*, int times);
	virtual void dispatchAfter(QRunnable*, const QTime& timeout);
	virtual void dispatchAfter(QRunnable*, dispatch_time_t timeout);
	virtual void dispatchSync(QRunnable*);
	virtual void setFinalizer(QRunnable*, QDispatchQueue*);
#ifdef HAS_BLOCKS
	virtual void dispatch(dispatch_block_t);
	virtual void dispatch(dispatch_iteration_block_t, int times);
	virtual void dispatchAfter(dispatch_block_t, const QTime& timeout);
	virtual void dispatchAfter(dispatch_block_t, dispatch_time_t timeout);
	virtual void dispatchSync(dispatch_block_t);
	virtual void setFinalizer(dispatch_block_t, QDispatchQueue*);
#endif
	virtual const QString label() const;
        virtual void suspend();
        virtual void resume();
	virtual const dispatch_queue_t nativeQueue() const;

private:
	dispatch_queue_t q;

	static void execRunnable(void*);
	static void execIterRunnable(void*, size_t);
	static void finalizeMe(void*);
};

#endif /* _DISPATCHQUEUEIMPLT_H_ */
