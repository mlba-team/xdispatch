
#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

class group::data {
public:
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

group::group(const group & other) : d(new data){
    assert(d);
    d->native = other.d->native;
    assert(d->native);
    dispatch_retain(d->native);
}

group::~group() {
    dispatch_release(d->native);
    delete d;
}

void group::async(operation* r, const queue& q){
    dispatch_queue_t nat_q = q.native();
    assert(nat_q);
    dispatch_group_async_f(d->native, nat_q, new wrap(r), run_wrap);
}

bool group::wait(dispatch_time_t time){
    return dispatch_group_wait(d->native, time) == 0;
}

bool group::wait(const time_t& t){
    return wait(as_dispatch_time(t));
}

void group::notify(operation* r, const queue& q){
    dispatch_queue_t nat_q = q.native();
    assert(nat_q);
    dispatch_group_notify_f(d->native, nat_q, new wrap(r), run_wrap);
}

const dispatch_group_t group::native() const {
    return d->native;
}

#ifdef XDISPATCH_HAS_BLOCKS

void group::async(dispatch_block_t b, const queue& q){
    dispatch_queue_t nat_q = q.native();
    assert(nat_q);
    dispatch_group_async(d->native, nat_q, b);
}

void group::notify(dispatch_block_t b, const queue& q){
    dispatch_queue_t nat_q = q.native();
    assert(nat_q);
    dispatch_group_notify_f(d->native, nat_q, new wrap(b), run_wrap);
}
#endif

std::ostream& xdispatch::operator<<(std::ostream& stream, const group* q){
    stream << "xdispatch::group";
    return stream;
}

std::ostream& xdispatch::operator<<(std::ostream& stream, const group& q){
    stream << "xdispatch::group";
    return stream;
}
