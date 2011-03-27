
#include <string>

#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

class queue::data {
public:
    dispatch_queue_t native;
    std::string label;
};

queue::queue(dispatch_queue_t q) : d(new data){
    assert(d);
    dispatch_retain(q);
    d->native = q;
    assert(d->native);
    d->label = std::string(dispatch_queue_get_label(q));
}

queue::queue(const queue& other) : d(new data){
    assert(d);
    d->native = other.d->native;
    d->label = other.d->label;
    assert(d->native);
    dispatch_retain(d->native);
}

queue::queue(const std::string & label) : d(new data){
    assert(d);
    d->native = dispatch_queue_create(label.c_str(),NULL);
    assert(d->native);
    d->label = label;
}

queue::queue(const char* label) : d(new data){
    assert(label);
    assert(d);
    d->native = dispatch_queue_create(label,NULL);
    assert(d->native);
    d->label = std::string(label);
}

queue::~queue(){
    dispatch_release(d->native);
    delete d;
}

void queue::async(operation* op){
    dispatch_async_f(d->native, new wrap(op), run_wrap);
}

void queue::apply(iteration_operation* op, size_t times){
    dispatch_apply_f(times, d->native, new iteration_wrap(op, times), run_iter_wrap);
}

void queue::after(operation* op, time_t time){
    after(op, as_dispatch_time(time) );
}

void queue::after(operation* op, dispatch_time_t time){
    dispatch_after_f(time, d->native, new wrap(op), run_wrap);
}

void queue::sync(operation* op){
    dispatch_sync_f(d->native, new wrap(op), run_wrap);
}

void queue::set_finalizer(operation* op, const queue& q){
    dispatch_set_finalizer_f(d->native, run_wrap);
    dispatch_set_context(d->native, new wrap(op));
    dispatch_set_target_queue(d->native, q.native());
}

const std::string queue::label() const {
    return d->label;
}

void queue::suspend(){
    dispatch_suspend(d->native);
}

void queue::resume(){
    dispatch_resume(d->native);
}

const dispatch_queue_t queue::native() const {
    return d->native;
}

#ifdef XDISPATCH_HAS_BLOCKS

void queue::async(dispatch_block_t b){
    dispatch_async(d->native, b);
}

void queue::apply(dispatch_iteration_block_t b, size_t times){
    dispatch_apply(times, d->native, b);
}

void queue::after(dispatch_block_t b, time_t time){
    dispatch_after(time, d->native, b);
}

void queue::after(dispatch_block_t b, dispatch_time_t time){
    dispatch_after(time, d->native, b);
}

void queue::sync(dispatch_block_t b){
    dispatch_sync(d->native, b);
}

void queue::set_finalizer(dispatch_block_t b, const queue& q){
    dispatch_set_finalizer_f(d->native, run_wrap);
    dispatch_set_context(d->native, new wrap(b));
    dispatch_set_target_queue(d->native, q.native());
}

#endif

std::ostream& xdispatch::operator<<(std::ostream& stream, const queue* q)
{
    stream << "xdispatch::queue (" << q->label() << ")";
    return stream;
}
std::ostream& xdispatch::operator<<(std::ostream& stream, const queue& q)
{
    stream << "xdispatch::queue (" << q.label() << ")";
    return stream;
}


bool xdispatch::operator ==(const queue& a, const queue& b){
    return a.native() == b.native();
}

bool xdispatch::operator ==(const dispatch_queue_t& a, const queue& b){
    return a == b.native();
}

bool xdispatch::operator ==(const queue& a, const dispatch_queue_t& b){
    return a.native() == b;
}
