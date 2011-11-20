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


#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

class group::data {
    public:
    data(){}
    data(const data& other) : native(other.native){
        assert(native);
        dispatch_retain(native);
    }
    ~data(){
        if(native)
            dispatch_release(native);
    }

    dispatch_group_t native;
};

group::group() : d(new data){
    assert(d);
    d->native = dispatch_group_create();
    assert(d->native);
}

group::group(dispatch_group_t g) : d(new data){
    assert(g);
    assert(d);
    dispatch_retain(g);
    d->native = g;
}

group::group(const group & other) : d(new data(*other.d)){
    assert(d);
    assert(d->native);
}

group::~group() {
    delete d;
}

void group::async(operation* r, const queue& q){
    dispatch_queue_t nat_q = (dispatch_queue_t)q.native();
    assert(nat_q);
    dispatch_group_async_f(d->native, nat_q, r, _xdispatch_run_operation);
}

bool group::wait(dispatch_time_t time){
    return dispatch_group_wait(d->native, time) == 0;
}

bool group::wait(struct tm* t){
    return wait(as_dispatch_time(t));
}

void group::notify(operation* r, const queue& q){
    dispatch_queue_t nat_q = (dispatch_queue_t)q.native();
    assert(nat_q);
    dispatch_group_notify_f(d->native, nat_q, r, _xdispatch_run_operation);
}

dispatch_object_t group::native() const {
    return d->native;
}

void group::suspend() {
    dispatch_suspend(d->native);
}

void group::resume() {
    dispatch_resume(d->native);
}

group& group::operator=(const group& other){
    if(*this != other){
        if(d)
            delete d;
        d = new data(*other.d);
        assert(d);
    }
    return *this;
}

std::ostream& xdispatch::operator<<(std::ostream& stream, const group* q){
    stream << "xdispatch::group";
    return stream;
}

std::ostream& xdispatch::operator<<(std::ostream& stream, const group& q){
    stream << "xdispatch::group";
    return stream;
}
