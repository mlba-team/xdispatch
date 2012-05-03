/*
* Copyright (c) 2008-2009 Apple Inc. All rights reserved.
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

#ifndef __HELP_BLOCKS_H_
#define __HELP_BLOCKS_H_

#ifdef __BLOCKS__
dispatch_block_t
_dispatch_Block_copy(dispatch_block_t db);
#define _dispatch_Block_copy(x) ((typeof(x))_dispatch_Block_copy(x))

void
_dispatch_call_block_and_release(void *block);

void
_dispatch_call_block_and_release2(void *block, void *ctxt);

#endif /* __BLOCKS__ */

#endif /* __HELP_BLOCKS_H_ */
