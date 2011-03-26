
#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

dispatch* dispatch::instance = new dispatch();

class dispatch::data {
public:
    queue* main;
    queue* global[3];
};

dispatch::dispatch() : d(new data) {
    d->main = new queue(dispatch_get_main_queue());
    d->global[LOW] = new queue(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW,NULL));
    d->global[DEFAULT] = new queue(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,NULL));
    d->global[HIGH] = new queue(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH,NULL));
}

dispatch::dispatch(const dispatch&){}

dispatch::~dispatch(){
    delete d->main;
    for(int i = 0; i < 3; i++)
        delete d->global[i];
    delete d;
}

queue* dispatch::create_queue(const std::string& label){
    return new queue(dispatch_queue_create(label.c_str(),NULL));
}

queue* dispatch::main_queue(){
    return d->main;
}

queue* dispatch::global_queue(Priority p){
    return d->global[p];
}

queue::a_ptr dispatch::current_queue(){
    return queue::a_ptr( new queue(dispatch_get_current_queue()) );
}

dispatch_time_t dispatch::as_dispatch_time(const time_t& t){
    return dispatch_time(t * NSEC_PER_SEC,0);
}

time_t dispatch::as_time_t(dispatch_time_t t){
    return t / NSEC_PER_SEC;
}
