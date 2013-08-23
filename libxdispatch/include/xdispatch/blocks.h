/*
* lambda_blocks.h
*
* Copyright (c) 2008-2009 Apple Inc.
* Copyright (c) 2011-2013 MLBA-Team.
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


#ifndef XDISPATCH_LAMBDA_BLOCKS_H_
#define XDISPATCH_LAMBDA_BLOCKS_H_

/**
 * @addtogroup xdispatch
 * @{
 */

#include "platform.h"
#include "dispatch_decl.h"

/**
  @def $
  Defines a cross platform way of writing code blocks (or lambdas).
  The block can take arguments and all local variables are available as
  read-only within the block. The variables values are the same as they were
  at runtime when declaring this block.

  The macro will either use c++0x lambda functions or blocks (as found in clang)
  depending on the individual availability. When you are sure, that you will use
  exactly one type of compiler only, you can use the original way of writing as
  well. All declared code (may it be blocks, lambdas or the cross-platform version)
  can be stored within a dispatch_block_t. This type of variable will be used
  throughout the entire xdispatch project.

  Currently supported compilers are:
  <ul>
    <li>clang 2.0</li>
    <li>MS Visual Studio 10</li>
    <li>gcc 4.5+ (with activated C++0x support)</li>
  </ul>

  If you're worried about namespace pollution, you can disable this macro by defining
  the following macro before including xdispatch/dispatch.h:

  @code
   #define XDISPATCH_NO_KEYWORDS
   #include <xdispatch/dispatch.h>
  @endcode

  Below you can find a small example for using the '$' macro:
  @code

  // the following code is working on all supported compilers equally
  dispatch_block_t myBlock = ${
        printf("Hello World!\n");
    };

  // working on clang 2.0 and the gcc 4.3 shipped with Mac OS 10.6
  dispatch_block_t myBlock = ^{
        printf("Hello World!\n");
    };

  // working on compilers with c++0x support (MSVC 10, gcc 4.5+)
  dispatch_block_t myBlock = []{
        printf("Hello Wolrd!\n");
    };

  // in all cases you can call the defined block in the same way:
  // -> output: "Hello World"
    myBlock();

  @endcode

  @remarks You can suppress features by adding one of the following
    definitions to your compiler:
    <ul>
        <li>
            XDISPATCH_NO_BLOCKS to completely disable block support
            regardless of the compiler support
        </li>
        <li>
            XDISPATCH_NO_LAMBDAS to completely disable C++11 lambda
            support regardless of the compiler support
        </li>
        <li>
            XDISPATCH_NO_FUNCTION to completely disable C++11 std::function
            support regardless of the compiler support. Please note
            this also disables lambda support automatically
        </li>
    </ul>

  @see XDISPATCH_BLOCK
  */


#if XDISPATCH_HAS_BLOCKS
 # include <Block.h>
 # include <stddef.h>
typedef void (^dispatch_block_t)(
    void
);
typedef void (^dispatch_iteration_block_t)(
    size_t
);
 # define XDISPATCH_BLOCK ^
 # ifndef XDISPATCH_NO_KEYWORDS
  #  define $ ^
 # endif

#elif XDISPATCH_HAS_LAMBDAS

 # define XDISPATCH_BLOCK [ = ]
 # ifndef XDISPATCH_NO_KEYWORDS
  #  define $ [ = ]
 # endif

#endif // if XDISPATCH_HAS_BLOCKS

/** @} */

#endif /* XDISPATCH_LAMBDA_BLOCKS_H_ */
