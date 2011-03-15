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



#include "../core/atomic.h"

#include <QRunnable>
#include "QDispatch.h"
#include "QBlockRunnable.h"
#include "QIterationRunnable.h"
#include "_DispatchQueueImpl.h"

struct IterationData {
        ATOMIC_INT count;
        QIterationRunnable* work;
};

_DispatchQueueImpl::_DispatchQueueImpl(dispatch_queue_t dq) : QDispatchQueue(), q(dq){
        if(dq==NULL)
                throw("Cannot construct queue");
}

_DispatchQueueImpl::_DispatchQueueImpl(const _DispatchQueueImpl& obj) : QDispatchQueue(), q(obj.q){
        if(obj.q==NULL)
                throw("Cannot construct queue");
        dispatch_retain(q);
}

_DispatchQueueImpl::~_DispatchQueueImpl(){
        dispatch_release(q);
}

void _DispatchQueueImpl::dispatch(QRunnable * runnable){
        dispatch_async_f(q,runnable,execRunnable);
}

void _DispatchQueueImpl::dispatch(QIterationRunnable * runnable, int times){
        IterationData* iter = new IterationData;
        iter->count = times;
        iter->work = runnable;
        dispatch_apply_f(times,q,iter,execIterRunnable);
}

void _DispatchQueueImpl::dispatchAfter(QRunnable *runnable, const QTime& t){
        dispatchAfter(runnable, QD->asDispatchTime(t));
}

void _DispatchQueueImpl::dispatchAfter(QRunnable* r, dispatch_time_t t){
        dispatch_after_f(t, q, r, execRunnable);
}

void _DispatchQueueImpl::dispatchSync(QRunnable * runnable){
        dispatch_sync_f(q, runnable, execRunnable);
}

void _DispatchQueueImpl::setFinalizer(QRunnable* r, QDispatchQueue* dq){
        Q_ASSERT(r!=NULL);

        if(dq != QD->getMainQueue()) {
                dispatch_set_finalizer_f(q, execRunnable);
                if(dq!=NULL) {
                        _DispatchQueueImpl* impl = static_cast<_DispatchQueueImpl*>(dq);
                        dispatch_set_target_queue(q, impl->nativeQueue());
                    }
        } else {
                dispatch_set_finalizer_f(q, finalizeMe);
        }
        dispatch_set_context(q, r);
}

#ifdef HAS_BLOCKS
void _DispatchQueueImpl::dispatch(dispatch_block_t block){
        dispatch_async(q, block);
}

void _DispatchQueueImpl::dispatch(dispatch_iteration_block_t block, int times){
        dispatch_apply(times, q, block);
}

void _DispatchQueueImpl::dispatchSync(dispatch_block_t block){
        dispatch_sync(q, block);
}

void _DispatchQueueImpl::dispatchAfter(dispatch_block_t block, const QTime& t){
        dispatch_after(QD->asDispatchTime(t), q, block);
}

void _DispatchQueueImpl::dispatchAfter(dispatch_block_t block, dispatch_time_t t){
        dispatch_after(t, q , block);
}

void _DispatchQueueImpl::setFinalizer(dispatch_block_t b, QDispatchQueue* q){
        setFinalizer(new QBlockRunnable(b), q);
}
#endif

void _DispatchQueueImpl::resume(){
        dispatch_resume(q);
}

void _DispatchQueueImpl::suspend(){
    dispatch_suspend(q);
}

const QString _DispatchQueueImpl::label() const {
        return QString(dispatch_queue_get_label(q));
}

const dispatch_queue_t _DispatchQueueImpl::nativeQueue() const {
        return q;
}

void _DispatchQueueImpl::execRunnable(void* context){
        if(context==NULL)
                return;
        QRunnable* r = (QRunnable*)context;
        r->run();
        if(r->autoDelete())
                delete r;
}

void _DispatchQueueImpl::execIterRunnable(void *context, size_t s){
        if(context==NULL)
                return;
        IterationData* r = static_cast<IterationData*>(context);
        r->work->run(s);
        if(atomic_dec_get(&r->count)==0){
                if(r->work->autoDelete())
                        delete r->work;
                delete r;
        }
}	

void _DispatchQueueImpl::finalizeMe(void* context){
        if(context==NULL)
                return;

        QRunnable* r = static_cast<QRunnable*>(context);
        r->setAutoDelete(true);
        QD->getMainQueue()->dispatch(r);
}
