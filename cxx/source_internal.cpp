/*
* Copyright (c) 2012 MLBA. All rights reserved.
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
#include "source_internal.h"


__XDISPATCH_USE_NAMESPACE

void _xdispatch_source_set_event_handler(dispatch_source_t src, xdispatch::operation* op){

    pointer<native_source_wrapper>::shared wrapper = native_source_wrapper::atomic_get ( src );
    wrapper->event_operation( op );

}

void _xdispatch_source_set_cancel_handler(dispatch_source_t src, xdispatch::operation* op){

    pointer<native_source_wrapper>::shared wrapper = native_source_wrapper::atomic_get ( src );
    wrapper->cancel_operation( op );

}

