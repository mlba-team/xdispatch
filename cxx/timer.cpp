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

__XDISPATCH_USE_NAMESPACE

class timer::data {
public:
    data(const xdispatch::queue& target)
        : native(NULL), interval(0), latency(0), handler(NULL), can_delete(true), parent(NULL),
           target(target) {}

    dispatch_source_t native;
    uint64_t interval;
    uint64_t latency;
    operation* handler;
    bool can_delete;
    timer* parent;
    xdispatch::queue target;

    inline void update_timeout(){
        dispatch_source_set_timer(native, DISPATCH_TIME_NOW, interval, latency);
    }

    static std::map<void*, timer::data*> handler_dates;
    static dispatch_once_t current_initialized;
    static pthread_key_t current;

    static void event_handler(void* context){
        assert(context);
        if(handler_dates.count(context) != 0) {
            timer::data* dt = handler_dates[context];
            if(dt->handler == NULL)
                return;

            dispatch_once_f(&current_initialized, NULL, init_current);
            timer* old = (timer*)pthread_getspecific(current);
            pthread_setspecific(current, dt->parent);
            (*dt->handler)();
            pthread_setspecific(current, old);
        }
    }

    static void init_current(void* unused){
        if(pthread_key_create(&current, NULL) ){
            perror("pthread_key_create()");
            abort();
        }
    }
};

std::map<void*, timer::data*> timer::data::handler_dates;
dispatch_once_t timer::data::current_initialized = 0;
pthread_key_t timer::data::current;

timer::timer(uint64_t i, const xdispatch::queue& q) : d(new data(q)){
    assert(d);
    d->parent = this;
    d->native = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, q.native());
    d->interval = i;
    dispatch_source_set_timer(d->native, DISPATCH_TIME_NOW, i, 0);
}

timer::timer(const timer& t) : d(new data(t.d->target)){
    assert(d);
    d->parent = this;
    d->native = t.d->native;
    d->interval = t.d->interval;
    d->latency = t.d->latency;
    assert(d->native);
    dispatch_retain(d->native);
}

timer::timer(dispatch_source_t src) : d(new data(xdispatch::global_queue())){
    assert(d);
    d->parent = this;
    d->native = src;
    assert(d->native);
    dispatch_retain(d->native);
}

timer::~timer(){
    dispatch_source_cancel(d->native);
    d->handler_dates.erase(d->native);
    dispatch_release(d->native);
    if(d->handler && d->can_delete)
        delete d->handler;
    delete d;
}

void timer::interval(uint64_t i){
    d->interval = i;
    d->update_timeout();
}

void timer::latency(uint64_t l){
    d->latency = l;
    d->update_timeout();
}

const dispatch_source_t timer::native() const {
    return d->native;
}

void timer::start(){
    dispatch_resume(d->native);
}

void timer::stop(){
    dispatch_suspend(d->native);
}

class delete_handler : public operation {
public:
    delete_handler(operation* h) : handler(h) {
        assert(h);
    }
    void operator ()(){
        delete handler;
    }

private:
    operation* handler;
};

void timer::handler(xdispatch::operation * op){
    // delete the old handler (but beware, it might still be in use)
    // TODO: This is not an optimal solution, as the handler might be deleted while still in use
    //       We need something similar to Qt's deleteLater()
    if(d->handler && d->can_delete)
        xdispatch::current_queue().async(new delete_handler(d->handler));

    // set the new handler
    d->can_delete = op->auto_delete();
    op->set_auto_delete(false);
    d->handler = op;
    d->handler_dates[d->native] = d;
    dispatch_set_context(d->native, d->native);
    dispatch_source_set_event_handler_f(d->native, data::event_handler);
}

void timer::set_queue(const xdispatch::queue &q){
    d->target = q;
    dispatch_set_target_queue(d->native, q.native());
}

xdispatch::queue timer::queue(){
    return d->target;
}

timer* timer::current(){
    dispatch_once_f(&data::current_initialized, NULL, data::init_current);
    return (timer*)pthread_getspecific(data::current);
}

void timer::single_shot(dispatch_time_t t, const xdispatch::queue & q, xdispatch::operation * op){
    xdispatch::queue(q).after(op, t);
}

void timer::single_shot(struct tm* t, const xdispatch::queue & q, xdispatch::operation * op){
    xdispatch::queue(q).after(op, t);
}

#ifdef XDISPATCH_HAS_BLOCKS

void timer::handler(dispatch_block_t b){
    handler(new xdispatch::block_operation(b));
}

void timer::single_shot(dispatch_time_t t, const xdispatch::queue & q, dispatch_block_t b){
    xdispatch::queue(q).after(b, t);
}

void timer::single_shot(struct tm* t, const xdispatch::queue & q, dispatch_block_t b){
    xdispatch::queue(q).after(b, t);
}
#endif

bool timer::operator ==(const timer& b){
    return d->native == b.d->native;
}
