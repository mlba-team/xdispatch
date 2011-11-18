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

#ifndef RUNBLOCKS_H_
#define RUNBLOCKS_H_

__XDISPATCH_BEGIN_NAMESPACE

class iteration_wrap {
    public:
        iteration_wrap(iteration_operation* o, size_t ct)
            : op(o), ref(ct) { assert(o); }
        ~iteration_wrap() {
            if(op) {
                if(op->auto_delete())
                    delete op;
            }
        }
        void run(size_t index){
            if(op)
                (*op)(index);
        }

        bool deref(){
            return dispatch_atomic_dec(&ref)==0;
        }

    private:
        iteration_operation* op;
        uintptr_t ref;
};

extern "C" {
void run_operation(void*);
void run_iter_wrap(void*, size_t);
}

__XDISPATCH_END_NAMESPACE

#endif /* RUNBLOCKS_H_ */
