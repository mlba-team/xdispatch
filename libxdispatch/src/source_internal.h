/*
* source_internal.h
*
* Copyright (c) 2011-2017 MLBA-Team
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


#ifndef XDISPATCH_SOURCE_INTERNAL_H_
#define XDISPATCH_SOURCE_INTERNAL_H_

#include "xdispatch_internal.h"

#include <map>

extern "C" void
native_source_wrapper__run_event_operation(
    void *
);

extern "C" void
native_source_wrapper__run_cancel_operation(
    void *
);

__XDISPATCH_BEGIN_NAMESPACE

/**
  An abstraction hiding the complexity of assigning a C++
  object as a handler for a dispatch_source_t. The wrapper
  will take posession of the passed operation and take care
  of its deletion. The same goes for the dispatch_source_t
  */
class native_source_wrapper
{
public:
    native_source_wrapper (
        dispatch_source_t source
    )
        : _source( source ),
          _op()
    {
        XDISPATCH_ASSERT( source );
        dispatch_retain( source );
    }

    native_source_wrapper (
        const native_source_wrapper &other
    )
        : _source( other._source ),
          _op( other._op )
    {
        XDISPATCH_ASSERT( _source );

        dispatch_retain( _source );
    }

    ~native_source_wrapper ()
    {
        dispatch_release( _source );
    }

    inline dispatch_source_t get() const
    {
        return _source;
    }

    inline dispatch_source_t operator -> () const
    {
        return get();
    }

    inline dispatch_source_t operator * () const
    {
        return get();
    }

    void event_operation(
        operation *op
    )
    {
        XDISPATCH_ASSERT( op );
        op->auto_delete( false );

        _op = pointer< operation >::shared( op );

        dispatch_set_context( _source, this );
        dispatch_source_set_event_handler_f( _source, native_source_wrapper__run_event_operation );
    }

    void cancel_operation(
        operation *op
    )
    {
        XDISPATCH_ASSERT( op );
        op->auto_delete( false );

        dispatch_set_context( _source, this );
        dispatch_source_set_cancel_handler_f( _source, native_source_wrapper__run_cancel_operation );

        _cancel_op = pointer< operation >::shared( op );
    }

    // we need to do some tricks here when using native source
    // objects as they cannot be passed a custom argument to their handler
    // function
    // TODO(marius): Still looking for a better solution as those objects will
    // never get deleted
    typedef std::map< dispatch_source_t, pointer< native_source_wrapper >::shared > source_wrapper_map;
    static source_wrapper_map xdispatch_source_wrappers;

    inline static pointer< native_source_wrapper >::shared atomic_get(
        dispatch_source_t obj
    )
    {
        synchronized {
            source_wrapper_map::iterator it = xdispatch_source_wrappers.find( obj );
            if( xdispatch_source_wrappers.end() == it )
            {
                it = xdispatch_source_wrappers.insert( xdispatch_source_wrappers.begin(),
                                                       std::make_pair( obj, pointer< native_source_wrapper >::shared( new native_source_wrapper( obj ) ) )
                                                     );
            }
            return it->second;
        }
        return pointer< native_source_wrapper >::shared();
    }

    dispatch_source_t _source;
    pointer< operation >::shared _op;
    pointer< operation >::shared _cancel_op;
};


__XDISPATCH_END_NAMESPACE

#endif /* XDISPATCH_SOURCE_INTERNAL_H_ */
