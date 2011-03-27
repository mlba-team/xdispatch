
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
  */
template <class T> class ptr_operation : public operation
{
public:
    ptr_operation(T* object, void(T::*function)())
        : obj(object), func(function) {}
    virtual void operator()() {
        (*obj.*func)();
    }

private:
    T* obj;
    void (T::*func)();
};

/**
  Provides a template functor to wrap
  a function pointer to a memberfunction of an object as iteration_operation
  */
template <class T> class ptr_iteration_operation : public iteration_operation
{
public:
    ptr_iteration_operation(T* object, void(T::*function)(size_t))
        : obj(object), func(function) {}
    virtual void operator()(size_t index) {
        (*obj.*func)(index);
    }

private:
    T* obj;
    void (T::*func)(size_t);
};


class queue;

/**
    Three priority classes used for the three standard
    global queues
    */
enum queue_priority { HIGH =2, DEFAULT =1, LOW =0 };
/**
    Returns the main queue. This is the queue running
    within the main thread. Thus normally only items put
    on this queue can change the GUI.
    @remarks Never delete the returned queue!
    @return NULL if something went wrong
    */
queue main_queue();
/**
    Returns the global queue associated to the given
    Priority p.

    Runnables submitted to these global concurrent queues
    may be executed concurrently with respect to
    each other.
    @remarks Never delete the returned queue!
    @return NULL if something went wrong
    */
queue global_queue(queue_priority p = DEFAULT);
/**
    @return The queue the current operation
        is executed on.
    */
queue current_queue();
/**
    @return The given QTime converted to a dispatch_time_t
    */
dispatch_time_t as_dispatch_time(const time_t&);
/**
    @return The given dispatch_time_t as time_t
    */
time_t as_time_t(dispatch_time_t t);
/**
    Enters the dispatching loop for the main thread.
    Call this somewhere within the main thread to enable
    dispatching operations to the main queue. Will never
    return.
    */
void exec();

__XDISPATCH_END_NAMESPACE

#endif /* XDISPATCH_BASE_H_ */
