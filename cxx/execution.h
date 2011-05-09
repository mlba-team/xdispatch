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

class wrap {
public:
#ifdef XDISPATCH_HAS_BLOCKS
    wrap(operation* o)
        : op(o) { assert(o); }
    wrap(dispatch_block_t b)
        : op(NULL) {
        blck = XDISPATCH_BLOCK_COPY(b);
    }
#else
    wrap(operation* o)
        : op(o) { assert(o); }
#endif
    ~wrap() {
        // std::cout << "OP deleted " << this << "( " << op << " )" << std::endl;
        if(op) {
            if(op->auto_delete())
                delete op;
        }
#ifdef XDISPATCH_HAS_BLOCKS
        else
            XDISPATCH_BLOCK_RELEASE(blck);
#endif
	}
    void run(){
        // std::cout << "OP starting " << this << "( " << op << " )" << std::endl;
        if(op)
            (*op)();
#ifdef XDISPATCH_HAS_BLOCKS
        else
            blck();
#endif
    }

private:
    operation* op;
#ifdef XDISPATCH_HAS_BLOCKS
    dispatch_block_t blck;
#endif
};

class iteration_wrap {
public:
#ifdef XDISPATCH_HAS_BLOCKS
    iteration_wrap(iteration_operation* o, size_t ct)
        : op(o), ref(ct), block(NULL) { assert(o); }
    iteration_wrap(const dispatch_iteration_block_t& b, size_t ct)
        : op(NULL), ref(ct), block(b) {}
#else
    iteration_wrap(iteration_operation* o, size_t ct)
        : op(o), ref(ct) { assert(o); }
#endif
    ~iteration_wrap() {
        if(op) {
			if(op->auto_delete())
				delete op;
		}
#ifdef XDISPATCH_HAS_BLOCKS
		else
            XDISPATCH_BLOCK_RELEASE(block);
#endif
    }
    void run(size_t index){
        if(op)
            (*op)(index);
#ifdef XDISPATCH_HAS_BLOCKS
        else
            block(index);
#endif
    }

    bool deref(){
        return dispatch_atomic_dec(&ref)==0;
    }

private:
    iteration_operation* op;
    ATOMIC_INT ref;
#ifdef XDISPATCH_HAS_BLOCKS
    dispatch_iteration_block_t block;
#endif
};

extern "C" {
    void run_iter_wrap(void*, size_t);
    void run_wrap(void*);
}

__XDISPATCH_END_NAMESPACE

#endif /* RUNBLOCKS_H_ */
