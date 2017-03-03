/*
* dispatch_decl.h
*
* Copyright (c) 2011-2017 MLBA-Team
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


#ifndef XDISPATCH_DECL_H_
#define XDISPATCH_DECL_H_

#include "platform.h"

#if XDISPATCH_COMPILER_MSVC2010 || XDISPATCH_COMPILER_MSVC2008SP1
 # pragma warning(disable: 4251) /* disable warning C4251 - * requires dll-interface */
 # ifdef XDISPATCH_MAKEDLL
  #  define XDISPATCH_EXPORT __declspec( dllexport )
 # else
  #  define XDISPATCH_EXPORT __declspec( dllimport )
 # endif
 # define XDISPATCH_DEPRECATED( F ) __declspec( deprecated ) F
#elif XDISPATCH_COMPILER_GCC || XDISPATCH_COMPILER_CLANG
 # define XDISPATCH_EXPORT __attribute__( ( __visibility__( "default" ) ) )
 # define XDISPATCH_DEPRECATED( F ) __attribute__ ( ( __deprecated__ ) ) F
#endif // if XDISPATCH_COMPILER_MSVC2010 || XDISPATCH_COMPILER_MSVC2008SP1


#define __XDISPATCH_BEGIN_NAMESPACE \
    namespace xdispatch \
    {
#define __XDISPATCH_END_NAMESPACE \
    }


/*
#ifdef _WIN32
# pragma warning(default: 4251) // re-enable warning C4251 - we do not want to influence other code
#endif
*/


#endif /* XDISPATCH_DECL_H_ */