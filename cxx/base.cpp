/*
* Copyright (c) 2011-2013 MLBA-Team. All rights reserved.
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


#ifdef _MSC_VER
# define _CRT_SECURE_NO_WARNINGS 1
# pragma warning (disable : 4996)
#endif

#include <iostream>
#include <time.h>

#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

object::object() {

}

object::~object() {

}

void object::resume () {
    dispatch_resume ( native() );
}

void object::suspend () {
    dispatch_suspend ( native() );
}

void object::target_queue (const queue & q) {
    dispatch_set_target_queue( native(), q.native_queue() );
}

bool object::operator==(const object& other){
    return native() == other.native();
}

bool object::operator==(const dispatch_object_t& other){
    return native() == other;
}

bool object::operator!=(const object& other){
    return native() != other.native();
}

bool object::operator!=(const dispatch_object_t& other){
    return native() != other;
}

bool operator ==(const dispatch_object_t& a, const object& b){
    return a == b.native();
}

bool operator !=(const dispatch_object_t& a, const object& b){
    return a == b.native();
}

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

    return as_delayed_time( (uint64_t)(diff*nsec_per_sec) );
}

struct tm xdispatch::as_struct_tm(const time& t){
    time_t rawtime = time(NULL);
    struct tm res;

    dispatch_time_t dtt = as_native_dispatch_time(t);
    res = *(localtime( &rawtime));

    res.tm_hour += (int)(dtt / (3600*nsec_per_sec));
    dtt %= (3600*(uint64_t)NSEC_PER_SEC);
    res.tm_min += (int)(dtt / (60*nsec_per_sec));
    dtt %= (60*(uint64_t)NSEC_PER_SEC);
    res.tm_sec += (int)(dtt / nsec_per_sec);

    return res;
}

void xdispatch::exec() {
    dispatch_main();
}
