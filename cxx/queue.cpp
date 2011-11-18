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


#include <string>

#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

class queue::data {
    public:
    data(){}
    data(const data& other)
        : native(other.native), label(other.label){
        assert(native);
        dispatch_retain(native);
    }
    ~data(){
        if(native)
            dispatch_release(native);
    }

    dispatch_queue_t native;
    std::string label;
};

queue::queue(dispatch_queue_t q) : d(new data){
    assert(d);
    dispatch_retain(q);
    d->native = q;
    assert(d->native);
    d->label = std::string(dispatch_queue_get_label(q));
}

queue::queue(const queue& other) : d(new data(*other.d)){
    assert(d);
}

queue::queue(const std::string & label) : d(new data){
    assert(d);
    d->native = dispatch_queue_create(label.c_str(),NULL);
    assert(d->native);
    d->label = label;
}

queue::queue(const char* label) : d(new data){
    assert(label);
    assert(d);
    d->native = dispatch_queue_create(label,NULL);
    assert(d->native);
    d->label = std::string(label);
}

queue::~queue(){
    delete d;
}

void queue::async(operation* op){
    dispatch_async_f(d->native, op, run_operation);
}

void queue::apply(iteration_operation* op, size_t times){
    dispatch_apply_f(times, d->native, new iteration_wrap(op, times), run_iter_wrap);
}

void queue::after(operation* op, struct tm* time){
    after(op, as_dispatch_time(time) );
}

void queue::after(operation* op, dispatch_time_t time){
    dispatch_after_f(time, d->native, op, run_operation);
}

void queue::sync(operation* op){
    dispatch_sync_f(d->native, op, run_operation);
}

void queue::finalizer(operation* op, const queue& q){
    dispatch_set_finalizer_f(d->native, run_operation);
    dispatch_set_context(d->native, op);
    dispatch_set_target_queue(d->native, (dispatch_queue_t)q.native());
}

const std::string queue::label() const {
    return d->label;
}

void queue::suspend(){
    dispatch_suspend(d->native);
}

void queue::resume(){
    dispatch_resume(d->native);
}

dispatch_object_t queue::native() const {
    return d->native;
}

void queue::target_queue(const queue& q) {
    dispatch_set_target_queue(d->native, (dispatch_queue_t)q.native());
}

xdispatch::queue& queue::operator=(const queue& other){
    if(*this != other){
        if(d)
            delete d;
        d = new data(*other.d);
        assert(d);
    }
    return *this;
}

std::ostream& xdispatch::operator<<(std::ostream& stream, const queue* q)
{
    stream << "xdispatch::queue (" << q->label() << ")";
    return stream;
}
std::ostream& xdispatch::operator<<(std::ostream& stream, const queue& q)
{
    stream << "xdispatch::queue (" << q.label() << ")";
    return stream;
}
