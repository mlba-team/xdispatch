#include <iostream>

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
    return queue(dispatch_get_global_queue(selection, 0));
}

queue xdispatch::current_queue(){
    return queue(dispatch_get_current_queue());
}

dispatch_time_t xdispatch::as_delayed_time(uint64_t delay, dispatch_time_t base){
    return dispatch_time(base,delay);
}

dispatch_time_t xdispatch::as_dispatch_time(struct tm* t){
    time_t now = time(NULL);
    time_t target = mktime(t);

    double diff = difftime(target, now);
    if(diff < 0) {
#ifdef DEBUG
        std::cerr << "as_dispatch_time: Passed time" << ctime(&target) << "is in the past, this not supported!" << std::endl;
#endif
        return as_delayed_time(0);
    }

    return as_delayed_time(diff*NSEC_PER_SEC );
}

struct tm xdispatch::as_struct_tm(dispatch_time_t t){
    time_t rawtime = time(NULL);
    struct tm res;

    res = *(localtime( &rawtime));

    res.tm_hour += t / (3600*NSEC_PER_SEC);
    t %= (3600*NSEC_PER_SEC);
    res.tm_min += t / (60*NSEC_PER_SEC);
    t %= (60*NSEC_PER_SEC);
    res.tm_sec += t / (NSEC_PER_SEC);

    return res;
}

void xdispatch::exec() {
    dispatch_main();
}
