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


#include <iostream>
#include <stdlib.h>
#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

iteration_wrap::iteration_wrap (
    iteration_operation *o,
    size_t ct
)
    : op( o ),
      ref( ct )
{
    XDISPATCH_ASSERT( o );
}


iteration_wrap::~iteration_wrap ()
{
    if( op && op->auto_delete() )
        delete op;
}


iteration_operation * iteration_wrap::operation()
{
    return op;
}


bool iteration_wrap::deref()
{
    return dispatch_atomic_dec( &ref ) == 0;
}


extern "C"
void _xdispatch_run_operation(
    void *dt
)
{
    XDISPATCH_ASSERT( dt );
    operation *w = static_cast< operation * > ( dt );
    XDISPATCH_ASSERT( w );

    try
    {
        ( *w )();
    }
    catch( const std::exception &e )
    {
        std::cerr << "##################################################################" << std::endl;
        std::cerr << "           Queue '" << xdispatch::current_queue().label() << "'" << std::endl;
        std::cerr << "xdispatch: Throwing exceptions within an xdispatch::operation is" << std::endl;
        std::cerr << "           not recommended, please make sure to catch them before:\n" << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "##################################################################" << std::endl;
        throw;
    }
    catch( ... )
    {
        std::cerr << "##################################################################" << std::endl;
        std::cerr << "           Queue '" << xdispatch::current_queue().label() << "'" << std::endl;
        std::cerr << "xdispatch: Throwing exceptions within an xdispatch::operation is" << std::endl;
        std::cerr << "           not recommended, please make sure to catch them before!" << std::endl;
        std::cerr << "##################################################################" << std::endl;
        std::terminate();
    }
    if( w->auto_delete() )
        delete w;
} // _xdispatch_run_operation


extern "C"
void _xdispatch_run_iter_wrap(
    void *dt,
    size_t index
)
{
    XDISPATCH_ASSERT( dt );
    iteration_wrap *wrap = static_cast< iteration_wrap * > ( dt );
    XDISPATCH_ASSERT( wrap );

    try
    {
        ( *( wrap->operation() ) )( index );
    }
    catch( const std::exception &e )
    {
        std::cerr << "##################################################################" << std::endl;
        std::cerr << "           Queue '" << xdispatch::current_queue().label() << "'" << std::endl;
        std::cerr << "xdispatch: Throwing exceptions within an xdispatch::operation is" << std::endl;
        std::cerr << "           not recommended, please make sure to catch them before:\n" << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "##################################################################" << std::endl;
        throw;
    }
    catch( ... )
    {
        std::cerr << "##################################################################" << std::endl;
        std::cerr << "           Queue '" << xdispatch::current_queue().label() << "'" << std::endl;
        std::cerr << "xdispatch: Throwing exceptions within an xdispatch::operation is" << std::endl;
        std::cerr << "           not recommended, please make sure to catch them before!" << std::endl;
        std::cerr << "##################################################################" << std::endl;
        std::terminate();
    }
} // _xdispatch_run_iter_wrap