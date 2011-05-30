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
#include <iostream>
#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

extern "C"
void xdispatch::run_operation(void* dt){  
    assert(dt);
    operation* w = static_cast<operation*>(dt);

    try {
        (*w)();
    } catch(const std::exception& e) {
        std::cerr << "Note: Throwing exceptions within an xdispatch::operation is not supported, please make sure to catch them before: " << e.what() << std::endl;
    } catch(...) {
        std::cerr << "Note: Throwing exceptions within an xdispatch::operation is not supported, please make sure to catch them before!" << std::endl;
    }

    if(w->auto_delete())
		delete w;
}

extern "C"
void xdispatch::run_iter_wrap(void* dt, size_t index){
    assert(dt);
    iteration_wrap* wrap = static_cast<iteration_wrap*>(dt);

    try {
        wrap->run(index);
    } catch(const std::exception& e) {
        std::cerr << "Note: Throwing exceptions within an xdispatch::operation is not supported, please make sure to catch them before: " << e.what() << std::endl;
    } catch(...) {
        std::cerr << "Note: Throwing exceptions within an xdispatch::operation is not supported, please make sure to catch them before!" << std::endl;
    }

    if(wrap->deref())
        delete wrap;
}
