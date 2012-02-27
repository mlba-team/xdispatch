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
#include "source_internal.h"

__XDISPATCH_USE_NAMESPACE

// used for storing the threadlocal data
static dispatch_once_t init_data_tls = 0;
static pthread_key_t data_tls;
std::map< dispatch_source_t, pointer<native_source_wrapper>::shared > native_source_wrapper::xdispatch_source_wrappers;


// sourcetype interface


static void run_tls_initializer(void*){
    pthread_key_create(&data_tls, NULL);
}

sourcetype::sourcetype() : cb(NULL) {

}

sourcetype::~sourcetype() {

}

void sourcetype::set_cb(source* s){
    XDISPATCH_ASSERT(s);

    (void)dispatch_atomic_ptr_xchg( &cb, s );
}

void sourcetype::on_resume (){

}

void sourcetype::on_suspend (){

}

void sourcetype::ready(const any& dt){

    source* callback = (source*)dispatch_atomic_ptr_xchg( &cb, cb );
    if( callback )
        callback->notify ( dt );
}

dispatch_source_t sourcetype::native(){
    return NULL;
}

void sourcetype::on_cancel() {

}


// sourcetype for using dispatch_source_t

native_source::native_source( dispatch_source_t s )
    : sourcetype(), _source( native_source_wrapper::atomic_get(s) ) {

    XDISPATCH_ASSERT( s );
    dispatch_retain( s );

    _source->event_operation(new ptr_operation<native_source>(this, &native_source::on_source_ready));
}

native_source::~native_source() {

}

dispatch_source_t native_source::native(){
    return _source->get();
}

void native_source::on_cancel () {
    dispatch_source_cancel ( _source->get() );
}

void native_source::on_resume () {
    dispatch_resume ( _source->get() );
}

void native_source::on_suspend () {
    dispatch_suspend ( _source->get() );
}

void native_source::on_source_ready () {
    ready ();
}

bool native_source::propagate_targetqueue () const {
    return true;
}


// the source class

class src_notify_operation : public operation {
    public:
        src_notify_operation(const any& dt, operation* op)
            : op(op), dt(dt){
            XDISPATCH_ASSERT(op);
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
        pdata(sourcetype* src_t)
            : suspend_ct(/* suspended by default */ 0), cancelled(0), type( src_t )
                ,target(global_queue()), handler(NULL), cancel_handler(NULL) {
            dispatch_once_f(&init_data_tls, NULL, run_tls_initializer);
            XDISPATCH_ASSERT(src_t);
        }

        ~pdata() {

            if( handler && handler->auto_delete() )
                delete handler;
            if( cancel_handler && cancel_handler->auto_delete() )
                delete cancel_handler;
        }

        inline bool fast_dispatch() const {
            return type->native() != NULL && type->propagate_targetqueue();
        }

        uintptr_t suspend_ct;
        uintptr_t cancelled;
        pointer<sourcetype>::unique type;
        queue target;
        operation* handler;
        operation* cancel_handler;
};

source::source(sourcetype* src_t) : d(new pdata(src_t)){
    XDISPATCH_ASSERT(d.get ());

    src_t->set_cb(this);
}

source::source(const source& other)
    : object(), d(other.d) {

}

source::~source(){


}

void source::suspend(){
    // if we reach 0, this means that 1 was
    // the old value, as such we need to suspend
    // the sourcetype as the state changed
    if( dispatch_atomic_dec(&d->suspend_ct) == 0 )
        d->type->on_suspend();
}

void source::resume(){
    // less than 1 means suspended, we only
    // need to signal to the sourcetype when
    // the state changed from 0 to 1
    if( dispatch_atomic_inc(&d->suspend_ct) == 1 )
        d->type->on_resume();
}

void source::target_queue(const queue& q){
    if( d->fast_dispatch() )
        dispatch_set_target_queue( native(), q.native_queue() );

    // TODO: Atomic?
    d->target = q;
}

queue source::target_queue() const {

    // TODO: Atomic?
    return d->target;
}



void source::handler(operation* op){
    XDISPATCH_ASSERT(op);

    operation* old_op = (operation*)dispatch_atomic_ptr_xchg( &d->handler, op );
    if( old_op && old_op->auto_delete() )
        delete old_op;
}



void source::notify(const any& dt){
    if(d->suspend_ct <= 0)
        return;

    // only run the handler if there is any and the source was not cancelled
    if( !d->handler || dispatch_atomic_cmpxchg( &d->cancelled, 1, 1) != 0 )
        return;

    if( d->fast_dispatch() )
        src_notify_operation(dt, d->handler).operator () ();
    else
        target_queue().async(new src_notify_operation(dt, d->handler));
}

dispatch_object_t source::native() const {
    return native_source();
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

sourcetype* source::source_type () {
    return d->type.get();
}

source& source::operator=(const source& other){
    if( (*this) != other ) {
        object::operator = ( other );
        d = other.d;
    }

    return *this;
}


void source::cancel_handler(operation * op) {
    XDISPATCH_ASSERT(op);

    operation* old_op = (operation*)dispatch_atomic_ptr_xchg( &d->cancel_handler, op );
    if( old_op && old_op->auto_delete() )
        delete old_op;

}



void source::cancel() {

    // we may only cancel once
    if( dispatch_atomic_xchg( &d->cancelled, 1 ) == 1 )
        return;

    // notify the source type
    d->type->on_cancel();

    // execute any cancel handlers
    operation* c_op = (operation*)dispatch_atomic_ptr_xchg( &d->cancel_handler, NULL );
    if( c_op )
        target_queue().async( c_op );
}
