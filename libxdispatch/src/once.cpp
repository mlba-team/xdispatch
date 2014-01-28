/*
* once.cpp
* 
* Copyright (c) 2011-2014 MLBA-Team
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


#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

// to be called by the once() handler

extern "C"
void _xdispatch_once_operation(
    void *blk
)
{
    operation *op = reinterpret_cast< operation * > ( blk );

    ( *op )();
}


// class implementation

void once::simple(
    dispatch_once_t *obj,
    dispatch_function_t func,
    void *data
)
{
    dispatch_once_f( obj, data, func );
}


void once::simple(
    once &obj,
    dispatch_function_t func,
    void *data
)
{
    dispatch_once_f( obj.native_once(), data, func );
}


once::once ()
    : _once_obj( 0 ),
      _once( &_once_obj ) { }


once::once (
    dispatch_once_t *dot
)
    : _once_obj( 0 ),
      _once( dot )
{
    XDISPATCH_ASSERT( dot );
}


void once::operator () (
    operation &op
)
{
    dispatch_once_f( _once, &op, _xdispatch_once_operation );
}


dispatch_once_t * once::native_once() const
{
    return _once;
}


std::ostream & xdispatch::operator << (
    std::ostream &stream,
    const once *q
)
{
    return operator << ( stream, *q );
}


std::ostream & xdispatch::operator << (
    std::ostream &stream,
    const once &q
)
{
    stream << "xdispatch::once (" << ( q._once == 0 ? "not" : "" ) << " executed)";
    return stream;
}