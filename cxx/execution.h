/*
* Copyright (c) 2011-2012 MLBA-Team. All rights reserved.
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

#ifndef XDISPATCH_INT_EXECUTION_H_
#define XDISPATCH_INT_EXECUTION_H_

extern "C" {
XDISPATCH_EXPORT void _xdispatch_run_operation(void*);
XDISPATCH_EXPORT void _xdispatch_run_iter_wrap(void*, size_t);
}

__XDISPATCH_BEGIN_NAMESPACE


class XDISPATCH_EXPORT iteration_wrap {
    public:
        iteration_wrap(iteration_operation* o, size_t ct);
        ~iteration_wrap();
        iteration_operation* operation();
        bool deref();

    private:
        iteration_operation* op;
        uintptr_t ref;
};


__XDISPATCH_END_NAMESPACE

#endif /* XDISPATCH_INT_EXECUTION_H_ */
