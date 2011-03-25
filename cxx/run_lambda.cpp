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


#include <assert.h>
#include "xdispatch_internal.h"

#ifdef XDISPATCH_HAS_BLOCKS

void run_block(void* block){
    assert(block);
    (*(dispatch_block_t*)block)();
#ifndef __BLOCKS__
    delete (dispatch_block_t*)block;
#else
    XDISPATCH_BLOCK_RELEASE(block);
#endif
}

void run_iteration_block(void* block, size_t s){
    assert(block);
    _block_function_t b = (_block_function_t)block;
    b->func(s);
    if(atomic_dec_get(&b->ref)==0)
#ifndef __BLOCKS__
        delete b;
#else
        XDISPATCH_BLOCK_RELEASE(block);
#endif
}

#endif
