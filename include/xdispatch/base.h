
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


#ifndef XDISPATCH_BASE_H_
#define XDISPATCH_BASE_H_

#ifndef __XDISPATCH_INDIRECT__
#error "Please #include <xdispatch/dispatch.h> instead of this file directly."
#endif

#include "dispatch.h"

#include <string>

__XDISPATCH_BEGIN_NAMESPACE

/**
  An operation is a functor used to
  define single portions of work to be
  dispatched to a single queue.

  Derive from this class and implement
  the operator to create specific operations
  that can be executed on a queue.
  */
class operation
{
public:
    operation() : auto_del(false){}

    virtual void operator()() = 0;
    /**
      Change the auto_delete flag to prevent
      the iteration from being deleted after
      finishing its execution. Defaults to true
      */
    virtual void set_auto_delete(bool a){ auto_del = a; }
    /**
      @return the current auto_delete flag
      @see set_auto_delete();
      */
    virtual bool auto_delete(){ return auto_del; }

private:
    bool auto_del;
};

/**
  @see operation

  Same as operation except that an
  index will be passed whenever this
  functor is executed on a queue.
  */
class iteration_operation
{
public:
    iteration_operation() : auto_del(false){}

    virtual void operator()(size_t index) = 0;
    /**
      Change the auto_delete flag to prevent
      the iteration from being deleted after
      finishing its execution. Defaults to true
      */
    virtual void set_auto_delete(bool a){ auto_del = a; }
    /**
      @return the current auto_delete flag
      @see set_auto_delete();
      */
    virtual bool auto_delete(){ return auto_del; }

private:
    bool auto_del;
};

/**
  Provides a template functor to wrap
  a function pointer to a memberfunction of an object as operation

  @note Pass NULL as object to encapsulate a single static function pointer
  */
template <class T> class ptr_operation : public operation
{
public:
    ptr_operation(T* object, void(T::*function)())
        : obj(object), func(function) {}
    virtual void operator()() {
        if(obj==NULL)
            (*func)();
        else
            (*obj.*func)();
    }

private:
    T* obj;
    void (T::*func)();
};

/**
  Provides a template functor to wrap
  a function pointer to a memberfunction of an object as iteration_operation

  @note Pass NULL as object to encapsulate a single static function pointer
  */
template <class T> class ptr_iteration_operation : public iteration_operation
{
public:
    ptr_iteration_operation(T* object, void(T::*function)(size_t))
        : obj(object), func(function) {}
    virtual void operator()(size_t index) {
        if(obj==NULL)
            (*func)(index);
        else
            (*obj.*func)(index);
    }

private:
    T* obj;
    void (T::*func)(size_t);
};

/**
  Use the XD macro as a quick accessor for the
  static instance of xdispatch::dispatch
  @see xdispatch::dispatch::instance
  */
#define XDISPATCH xdispatch::dispatch::instance

class queue;

/**
Single Instance Interface to control the dispatch behaviour.
Use this object to create new queues, get one of the main or
global queues.

When using this object from within an operation executed on a
queue you can also use it to get the queue you're running in.

For information about the Dispatch mechanism, please
see Apple's documentation on libDispatch as well.
*/
class dispatch {

public:

    /**
    Three priority classes used for the three standard
    global queues
    */
    enum Priority { HIGH =2, DEFAULT =1, LOW =0 };
    /**
    Use this to access the dispatch instance
    of your program
    */
    static dispatch* instance;
    /**
    Create a new serial queue with the given label.
    All items dispatched to this queue will be executed
    in FIFO order. Please note that still items in different
    queues can and will be executed concurrently.

    It is your own responsibility to delete the queue
    when no longer needed.

    @return NULL if something went wrong
    */
    queue* create_queue(const std::string& label);
    /**
    Returns the main queue. This is the queue running
    within the main thread. Thus normally only items put
    on this queue can change the GUI.
    @remarks Never delete the returned queue!
    @return NULL if something went wrong
    */
    queue* main_queue();
    /**
    Returns the global queue associated to the given
    Priority p.

    Runnables submitted to these global concurrent queues
    may be executed concurrently with respect to
    each other.
    @remarks Never delete the returned queue!
    @return NULL if something went wrong
    */
    queue* global_queue(Priority p = DEFAULT);
    /**
    @return The queue this runnable is executed in or NULL if
    this function is not called from a QRunnable executed
    in a queue.

    @remarks The value returned here is wrapped within
        queue::a_ptr which is a typedef for
        std::auto_ptr. This has the benefit of calling
        current_queue() without creating any leaks or a
        need to call delete. To control the lifetime of
        the queue object yourself, please use
        a_ptr.release() and have a look at the documentation
        of std::auto_ptr
    @see queue::a_ptr
    */
    std::auto_ptr< queue > current_queue();
    /**
    @return The given QTime converted to a dispatch_time_t
    */
    static dispatch_time_t as_dispatch_time(const time_t&);
    /**
    @return The given dispatch_time_t as time_t
    */
    static time_t as_time_t(dispatch_time_t t);
    /**
    Enters the dispatching loop for the main thread.
    Call this somewhere within the main thread to enable
    dispatching operations to the main queue. Will never
    return.
    */
    static void exec();

private:
    dispatch();
    dispatch(const dispatch&);
    ~dispatch();

    class data;
    data* d;

};

__XDISPATCH_END_NAMESPACE

#endif /* XDISPATCH_BASE_H_ */
