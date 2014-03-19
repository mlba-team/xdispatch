/*
* platform.h
* 
* Copyright (c) 2011-2014 MLBA-Team
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

#ifndef XDISPATCH_PLATFORM_H_
#define XDISPATCH_PLATFORM_H_


// detect compilers

#if defined __clang__
 # define XDISPATCH_COMPILER_CLANG 1
 # define XDISPATCH_COMPILER "clang"
#endif

#if defined __GNUC__ && ( !defined __clang__ )
 # define XDISPATCH_COMPILER_GCC 1
 # define XDISPATCH_COMPILER "gcc"
#endif

#if _MSC_VER >= 1600
 # define XDISPATCH_COMPILER_MSVC2010 1
 # define XDISPATCH_COMPILER "Visual Studio 2010"
#elif _MSC_VER >= 1500
 # define XDISPATCH_COMPILER_MSVC2008SP1 1
 # define XDISPATCH_COMPILER "Visual Studio 2008 SP1"
#endif // if _MSC_VER >= 1600

#ifndef XDISPATCH_COMPILER
 # error "Unsupported compiler version"
#endif


// detect enabled featureset


#if defined __BLOCKS__
 # define XDISPATCH_HAS_BLOCKS 1
#endif

#if __cplusplus
 # include <cstddef>

 # if defined _LIBCPP_VERSION
  #  define XDISPATCH_STL_LIBCPP 1
  #  define XDISPATCH_STL "libc++"
 # elif defined __GLIBCXX__
  #  define XDISPATCH_STL_GLIBCXX 1
  #  define XDISPATCH_STL "libstdc++"
 # endif // if defined _LIBCPP_VERSION

 # if ( __cplusplus >= 201103L )
  #  define XDISPATCH_CPP11 1
 # endif

 # if ( defined __GXX_EXPERIMENTAL_CXX0X_ )
  #  define XDISPATCH_CPP11 1
 # endif

 # if XDISPATCH_COMPILER_MSVC2010
  #  define XDISPATCH_CPP11 1
  #  define XDISPATCH_CPP11_TYPE_TRAITS 1
  #  define XDISPATCH_CPP11_MEMORY 1
  #  define XDISPATCH_CPP11_FUNCTIONAL 1
 # elif XDISPATCH_COMPILER_MSVC2008SP1
  #  define XDISPATCH_TR1_FUNCTIONAL 1
  #  include <functional>
 # endif // if XDISPATCH_COMPILER_MSVC2008SP1

 # if XDISPATCH_CPP11
  #  define XDISPATCH_HAS_LAMBDAS 1
 # endif

 # if XDISPATCH_STL_LIBCPP && XDISPATCH_CPP11
  #  define XDISPATCH_CPP11_TYPE_TRAITS 1
  #  define XDISPATCH_CPP11_MEMORY 1
  #  define XDISPATCH_CPP11_FUNCTIONAL 1
 # elif XDISPATCH_CPP11 && __GLIBCXX__ >= 20120322
  #  define XDISPATCH_CPP11_TYPE_TRAITS 1
  #  define XDISPATCH_CPP11_MEMORY 1
  #  define XDISPATCH_CPP11_FUNCTIONAL 1
 # elif __GLIBCXX__ < 20120322 && ( !XDISPATCH_CPP11 )
  #  define XDISPATCH_TR1_FUNCTIONAL 1
  #  include <tr1/functional>
 # endif // if __GLIBCXX__ < 20120322

 # if XDISPATCH_TR1_FUNCTIONAL || XDISPATCH_CPP11_FUNCTIONAL
  #  define XDISPATCH_HAS_FUNCTION 1
 # endif

#endif // if __cplusplus

// user control of enabled featureset

#if defined XDISPATCH_NO_BLOCKS
 # undef XDISPATCH_HAS_BLOCKS
#endif

#if defined XDISPATCH_NO_LAMBDAS
 # undef XDISPATCH_HAS_LAMBDAS
#endif

#if defined XDISPATCH_NO_FUNCTION
 # undef XDISPATCH_HAS_LAMBDAS
 # undef XDISPATCH_HAS_FUNCTION
#endif // if defined XDISPATCH_NO_FUNCTION

#endif // XDISPATCH_PLATFORM_H_
