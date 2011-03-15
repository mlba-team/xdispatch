/*
* Copyright (c) 2010-2011 MLBA. All rights reserved.
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

#ifndef BLOCKS_H_
#define BLOCKS_H_

#define _STR(A) #A
#define _TOSTR(A) _STR(A)

/**
  Defines a new code block. The block can take arguments
  and all local variables are available as read-only within
  the block. The variables values are the same as they were
  when creating this block

  @param CODE Your code embraced in brackets, e.g. {printf("Hello World");}

  @code

  // A block example
  dispatch_block_t myBlock = ${
		printf("Hello World!\n");
	};

  // now call this block -> output: "Hello World"
	myBlock();

  @endcode
  */

#ifdef __cplusplus
#	ifdef __BLOCKS__
#		include <Block.h>
#		include <stddef.h>
#		define $ ^ // $(CODE) ^CODE
		//typedef void (dispatch_block_t)(void);
		typedef void (^dispatch_iteration_block_t)(size_t);
#		define HAS_BLOCKS
#		if defined(__cplusplus) && !defined(__clang__)
#			warning "Sadly blocks are currently broken in C++ on this platform, we recommend using gcc 4.5.1 or clang 2 instead"
#		endif
#	elif _MSC_VER >= 1600
#		include <functional>
#		define $ [=] // $(CODE) [=]CODE
#		define Block_copy(A) A
#		define Block_release(A)
		/**
			A dispatch_block_t is a lambda function taking no
			arguments at all, i.e. declared as follows:
				${ // do something }
		*/
		typedef std::tr1::function< void (void) > dispatch_block_t;
		/**
			A dispatch_iteration_block_t is a lambda function taking one
			argument of size_t for indicating the current position in the
			iteration, i.e. declared as follows:
				$(size_t pos){ // do something }
		*/
		typedef std::tr1::function< void (size_t) > dispatch_iteration_block_t;
#		define HAS_BLOCKS
#	elif defined __GXX_EXPERIMENTAL_CXX0X__
#		include <tr1/functional>
#		define $ [=] // $(CODE) [=]CODE
#		define Block_copy(A) A
#		define Block_release(A)
		typedef std::tr1::function< void (void) > dispatch_block_t;
		typedef std::tr1::function< void (size_t) > dispatch_iteration_block_t;
#		define HAS_BLOCKS
#	else
#		ifdef _GNUC__
#			warning "Blocks support not available on this platform"
#		else
#			pragma message("Blocks support not available on this platform")
#		endif	
#	endif
#endif

/*#ifdef HAS_BLOCKS
#		ifdef _GNUC__ 
#			warning "Have Blocks support"
#		else
#			pragma message("Have Blocks support")
#		endif
#endif */

#endif /* BLOCKS_H_ */
