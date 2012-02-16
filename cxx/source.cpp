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
#include <exception>
#include <stdexcept>

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
    // TODO: Is this synchronization really needed?
    //       Also check if maintaining an own object is faster
    synchronize("xd_st_cb") {
        cb = s;
    }
}

void sourcetype::ready(const any& dt){
    // TODO: Is this synchronization really needed?
    //       Also check if maintaining an own object is faster
    synchronize("xd_st_cb") {
        if(cb)
            cb->notify(dt);
    }
}

dispatch_source_t sourcetype::native(){
    return NULL;
}

void sourcetype::on_cancel() {

}


// sourcetype for using dispatch_source_t

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



// the source class

class src_notify_operation : public operation {
    public:
        src_notify_operation(const any& dt, operation* op)
            : op(op), dt(dt){
            assert(op);
        }

        void operator()(){
            pthread_setspecific(data_tls, &dt);
            (*op)();
            pthread_setspecific(data_tls, NULL);
        }

    private:
        operation* op;
        any dt;
};

class source::pdata {
    public:
        pdata()
            : target(global_queue()), handler(NULL), cancel_handler(NULL) {
            dispatch_once_f(&init_data_tls, NULL, run_tls_initializer);
        }

        uintptr_t suspend_ct;
        uintptr_t cancelled;
        sourcetype* type;
        queue target;
        operation* handler;
        operation* cancel_handler;
};

source::source(sourcetype* src_t) : d(new pdata()){
    assert(d);
    assert(src_t);
    d->suspend_ct = 1;
    d->cancelled = 0;
    d->type = src_t;
    src_t->set_cb(this);
}

source::source(const source&) {

}

source::~source(){
    delete d->type;

    if( d->handler && d->handler->auto_delete() )
        delete d->handler;
    if( d->cancel_handler && d->cancel_handler->auto_delete() )
        delete d->cancel_handler;

    delete d;
}

void source::suspend(){
    if( native() )
        dispatch_suspend( native() );
    else
        dispatch_atomic_dec(&d->suspend_ct);
}

void source::resume(){
    if( native() )
        dispatch_resume( native() );
    else
        dispatch_atomic_inc(&d->suspend_ct);
}

void source::target_queue(const queue& q){
    if( native() )
        dispatch_set_target_queue( native(), q.native_queue() );

    d->target = q;
}

queue source::target_queue() const {
    return d->target;
}




// we need to do some tricks here when using native source
// objects as they cannot be passed a custom argument to their handler function
// TODO: operations in there might never get deleted
static std::map< dispatch_object_t, operation* > xdispatch_source_handlers;
extern "C" void xdispatch_source_run_handler(void* data){

    dispatch_object_t object = (dispatch_object_t)(data);
    if( xdispatch_source_handlers.count(object) == 0 )
        return;

    operation* op = xdispatch_source_handlers.at( object );
    if( !op )
        return;

    op->operator ()();
}

void source::handler(operation* op){
    assert(op);

    if( native() ) {
        xdispatch_source_handlers[ native() ] = op;
        dispatch_set_context( native(), native() );
        dispatch_source_set_event_handler_f( native_source(), xdispatch_source_run_handler );
        return;
    }

    // TODO: Is this synchronization really needed?
    //       Also check if maintaining an own object is faster
    synchronize("xd_src_handle") {
        if(d->handler && d->handler->auto_delete())
            delete d->handler;

        d->handler = op;
    }
}



void source::notify(const any& dt){
    if(d->suspend_ct == 0)
        return;

    // only run the handler if there is any and the source was not cancelled
    if(d->handler && dispatch_atomic_cmpxchg( &d->cancelled, 1, 1) == 0 )
        target_queue().async(new src_notify_operation(dt, d->handler));
}

dispatch_object_t source::native() const {
    return d->type->native();
}

dispatch_source_t source::native_source() const {
    return d->type->native();
}

const any* source::_data(){
    const any* dt_pt = (any*)pthread_getspecific(data_tls);

    if(dt_pt == NULL)
        throw std::runtime_error("xdispatch::source::data() - There is no data available for you.");

    return dt_pt;
}

source& source::operator=(const source&){
    return *this;
}



// we need to do some tricks here when using native source
// objects as they cannot be passed a custom argument to their handler function
static std::map< dispatch_object_t, operation* > xdispatch_source_cancel_handlers;
extern "C" void xdispatch_source_run_cancel_handler(void* data){

    dispatch_object_t object = (dispatch_object_t)(data);
    if( xdispatch_source_cancel_handlers.count(object) == 0 )
        return;

    operation* op = xdispatch_source_cancel_handlers.at( object );
    xdispatch_source_cancel_handlers.erase( object );
    if( !op )
        return;

    op->operator ()();
    if( op->auto_delete() )
        delete op;
}

void source::cancel_handler(operation * op) {

    if( native() ) {
        xdispatch_source_cancel_handlers[ native() ] = op;
        dispatch_set_context( native(), native() );
        dispatch_source_set_cancel_handler_f( native_source(), xdispatch_source_run_cancel_handler );
        return;
    }

    synchronize("xd_src_cancel") {
        if(d->cancel_handler && d->cancel_handler->auto_delete())
            delete d->cancel_handler;

        d->cancel_handler = op;
    }
}



void source::cancel() {

    // we may only cancel once
    if( dispatch_atomic_xchg( &d->cancelled, 1 ) == 1 )
        return;

    // notify the source type
    d->type->on_cancel();

    // use the native mechanisms if any
    if( native() ) {
        dispatch_source_cancel( native_source() );
        return;
    }

    // execute any cancel handlers
    synchronize("xd_src_cancel") {
        if( d->cancel_handler ) {
            target_queue().async( d->cancel_handler );
            d->cancel_handler = NULL;
        }
    }
}
