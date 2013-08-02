/*
* once.h
*
* Copyright (c) 2011-2013 MLBA-Team.
* All rights reserved.
*
* @LICENSE_HEADER_START@
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
* @LICENSE_HEADER_END@
*/


#ifndef XDISPATCH_ONCE_H_
#define XDISPATCH_ONCE_H_

/**
 * @addtogroup xdispatch
 * @{
 */

#ifndef __XDISPATCH_INDIRECT__
 # error "Please #include <xdispatch/dispatch.h> instead of this file directly."
#endif

#include <iostream>

__XDISPATCH_BEGIN_NAMESPACE

class operation;

/**
  Provides a mean to execute some job
  exactly once during the lifetime
  of a programm and using multiple threads.

  This can be very handy
  when e.g. allocating resources.

  @see dispatch_once
  */
class XDISPATCH_EXPORT once
{
public:
    /**
     * Method for running a simple function pointer on a once object
     */
    static void simple( dispatch_once_t *, dispatch_function_t, void * = NULL );
    /**
     * Method for running a simple function pointer on a once object
     */
    static void simple( once &, dispatch_function_t, void * = NULL );
    /**
      Creates a new once object, marked
      as not having been executed yet
      */
    once ();
    /**
      Creates a new once object, the
      execution state is shared with
      the given dispatch_once_t object.
      */
    once (
        dispatch_once_t *
    );

    /**
     @returns the native dispatch object associated to
     the xdispatch object
     */
    dispatch_once_t * native_once() const;

    /**
      Executes the given operation when the
      once object has not executed any operation
      before, both the current or any other thread.

      @remarks In constrast to the other xdispatch
        classes, this method receives a reference to
        operations only and will not take posession
        of the operation
        */
    void operator () (
        operation &
    );

    /**
      Similar to operator()(operation&)

      Will wrap the given function as operation
      and try to execute it on the once object

      @see operator()(operation&)
      */
    template< typename _Func >
    inline typename std::enable_if< std::is_base_of< operation, _Func >::value, void >::type operator () (
        _Func &b
    )
    {
        once_op< _Func > op( b );

        operator () ( static_cast< operation & > ( op ) );
    }

    template< typename _Func >
    inline typename std::enable_if< !std::is_base_of< operation, _Func >::value, void >::type operator () (
        const _Func &b
    )
    {
        once_op< const _Func > op( b );

        operator () ( static_cast< operation & > ( op ) );
    }

private:
    dispatch_once_t _once_obj;
    dispatch_once_t *_once;

    // we define our own operation class
    // as the function_operation does a
    // copy of the stored function, something
    // we do not need in here
    template< typename _Func >
    class once_op
        : public operation
    {
public:
        once_op (
            _Func &b
        )
            : operation(),
              _func( b ) { }


        void operator () ()
        {
            _func();
        }

private:
        _Func &_func;
    };


    friend XDISPATCH_EXPORT std::ostream & operator << (
        std::ostream &,
        const once &
    );
};


XDISPATCH_EXPORT std::ostream &
operator << (
    std::ostream &,
    const once *
);

XDISPATCH_EXPORT std::ostream &
operator << (
    std::ostream &,
    const once &
);

__XDISPATCH_END_NAMESPACE

/** @} */

#endif /* XDISPATCH_ONCE_H_ */