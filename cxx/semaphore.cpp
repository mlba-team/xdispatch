
#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

class semaphore::data {
public:
    dispatch_semaphore_t native;
};

semaphore::semaphore(int val) : d(new data) {
    assert(d);
    d->native = dispatch_semaphore_create(val);
    assert(d->native);
}

semaphore::semaphore(dispatch_semaphore_t sem) : d(new data) {
    assert(sem);
    assert(d);
    d->native = sem;
    dispatch_retain(sem);
}


semaphore::semaphore(const semaphore& other) : d(new data) {
    assert(d);
    d->native = other.d->native;
    dispatch_retain(d->native);
}

semaphore::~semaphore() {
    dispatch_release(d->native);
    delete d;
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

bool semaphore::try_acquire(const time_t& time){
    return try_acquire( as_dispatch_time(time) );
}

const dispatch_semaphore_t semaphore::native() const {
    return d->native;
}

std::ostream& xdispatch::operator<<(std::ostream& stream, const semaphore* q){
    stream << "xdispatch::semaphore";
    return stream;
}

std::ostream& xdispatch::operator<<(std::ostream& stream, const semaphore& q){
    stream << "xdispatch::semaphore";
    return stream;
}
