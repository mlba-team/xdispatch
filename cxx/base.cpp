
#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

queue xdispatch::main_queue(){
    return queue(dispatch_get_main_queue());
}

queue xdispatch::global_queue(queue_priority p){
    long selection;
    switch(p) {
    case LOW:
        selection = DISPATCH_QUEUE_PRIORITY_LOW;
        break;
    case HIGH:
        selection = DISPATCH_QUEUE_PRIORITY_HIGH;
        break;
    default:
        selection = DISPATCH_QUEUE_PRIORITY_DEFAULT;
    }
    return queue(dispatch_get_global_queue(selection, NULL));
}

queue xdispatch::current_queue(){
    return queue(dispatch_get_current_queue());
}

dispatch_time_t xdispatch::as_dispatch_time(const time_t& t){
    return dispatch_time(t * NSEC_PER_SEC,0);
}

time_t xdispatch::as_time_t(dispatch_time_t t){
    return t / NSEC_PER_SEC;
}

void xdispatch::exec() {
    dispatch_main();
}
