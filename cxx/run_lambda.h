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

#ifdef XDISPATCH_HAS_BLOCKS

typedef struct _block_function_s {
public:
    dispatch_iteration_block_t func;
    unsigned int ref;

    _block_function_s(const dispatch_iteration_block_t &b, size_t ct) : func(b), ref(ct){}

} * _block_function_t;

void run_block(void* block);
void run_iteration_block(void* block, size_t s);

#endif

#endif /* RUNBLOCKS_H_ */
