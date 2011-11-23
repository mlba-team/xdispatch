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

// sourcetype interface

static dispatch_once_t init_data_tls = 0;
static pthread_key_t data_tls;

static void run_tls_initializer(void*){
    pthread_key_create(&data_tls, NULL);
}

sourcetype::sourcetype() : cb(NULL) {

}

sourcetype::~sourcetype() {

}

void sourcetype::set_cb(source* s){
    assert(s);
    cb = s;
}

void sourcetype::ready(const any& dt){
    if(cb)
        cb->notify(dt);
}

dispatch_source_t sourcetype::native(){
    return NULL;
}


// sourcetype for using dispatch_source_t

/*

native_source::native_source( dispatch_source_t s )
    : sourcetype(), _source( s ) {

    assert( s );
    dispatch_retain( s );
}

native_source::~native_source() {
    dispatch_release( _source );
}

dispatch_source_t native_source::native(){
    return _source;
}

*/

// the source class

class src_notify_operation : public operation {
    public:
        src_notify_operation(const any& dt, operation* op) : op(op), dt(dt){
            assert(op);
        }

        void operator()(){
            pthread_setspecific(data_tls, &dt);
            (*op)();
            pthread_setspecific(data_tls, NULL);

            if(op->auto_delete())
                delete op;
        }

    private:
        operation* op;
        any dt;
};

class source::pdata {
    public:
        pdata() : target(global_queue()), handler(NULL) {
            dispatch_once_f(&init_data_tls, NULL, run_tls_initializer);
        }

        uintptr_t suspend_ct;
        sourcetype* type;
        queue target;
        operation* handler;
};

source::source(sourcetype* src_t) : d(new pdata()){
    assert(d);
    assert(src_t);
    d->suspend_ct = 1;
    d->type = src_t;
    src_t->set_cb(this);
}

source::source(const source&) {}

source::~source(){
    delete d->type;
    if( d->handler )
        delete d->handler;
    delete d;
}

void source::suspend(){
    dispatch_atomic_dec(&d->suspend_ct);
}

void source::resume(){
    dispatch_atomic_inc(&d->suspend_ct);
}

void source::target_queue(const queue& q){
    d->target = q;
}

queue source::target_queue() const {
    return d->target;
}

void source::handler(operation* op){
    assert(op);

    if(d->handler)
        delete d->handler;

    d->handler = op;
    d->handler->auto_delete(false);
}

void source::notify(const any& dt){
    if(d->suspend_ct == 0)
        return;

    d->target.async(new src_notify_operation(dt, d->handler));
}

dispatch_object_t source::native() const {
    return d->type->native();
}

const any* source::_data(){
    const any* dt_pt = (any*)pthread_getspecific(data_tls);
    if(dt_pt == NULL)
        throw std::exception();
    return dt_pt;
}

source& source::operator=(const source&){
    return *this;
}
