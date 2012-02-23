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

class semaphore::data {
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

    dispatch_semaphore_t native;
};

semaphore::semaphore(int val) : d(new data) {
    assert(d.get ());
    d->native = dispatch_semaphore_create(val);
    assert(d->native);
}

semaphore::semaphore(dispatch_semaphore_t sem) : d(new data) {
    assert(sem);
    assert(d.get ());
    d->native = sem;
    dispatch_retain(sem);
}


semaphore::semaphore(const semaphore& other) : d(new data(*other.d)) {
    assert(d.get ());
}

semaphore::~semaphore() {

}

int semaphore::release() {
    return dispatch_semaphore_signal(d->native);
}

void semaphore::acquire() {
    dispatch_semaphore_wait(d->native, DISPATCH_TIME_FOREVER);
}

bool semaphore::try_acquire(dispatch_time_t time) {
    return dispatch_semaphore_wait(d->native, time) == 0;
}

bool semaphore::try_acquire(struct tm* time){
    return try_acquire( as_dispatch_time(time) );
}

const dispatch_semaphore_t semaphore::native_semaphore() const {
    return d->native;
}

xdispatch::semaphore& semaphore::operator=(const semaphore& other){
    if(*this != other){
        d = pointer<data>::unique( new data(*other.d) );
        assert(d.get ());
    }
    return *this;
}

bool semaphore::operator==(const semaphore& other){
    return d->native == other.d->native;
}

bool semaphore::operator!=(const semaphore& other){
    return d->native != other.d->native;
}

bool semaphore::operator==(const dispatch_semaphore_t& other){
    return d->native == other;
}

bool semaphore::operator!=(const dispatch_semaphore_t& other){
    return d->native != other;
}

std::ostream& xdispatch::operator<<(std::ostream& stream, const semaphore* q){
    stream << "xdispatch::semaphore";
    return stream;
}

std::ostream& xdispatch::operator<<(std::ostream& stream, const semaphore& q){
    stream << "xdispatch::semaphore";
    return stream;
}

