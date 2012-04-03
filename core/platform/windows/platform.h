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

#ifndef WINDOWS_PLATFORM_H_
#define WINDOWS_PLATFORM_H_

/* Workaround for a bug in VS2008 */
#if _MSC_VER < 1600
# ifndef _IVEC_H_INCLUDED
#  define _IVEC_H_INCLUDED 1
# endif
# ifndef _FVEC_H_INCLUDED
#  define _FVEC_H_INCLUDED 1
# endif
# ifndef _DVEC_H_INCLUDED
#  define _DVEC_H_INCLUDED 1
# endif
#endif

#include "posix.h"
#include "threads.h"
#include "queue.h"

#if !defined(__GNUC__) && _MSC_VER < 1600
# include "stdint.h"
#else
# include <stdint.h>
#endif

#endif /* WINDOWS_PLATFORM_H_ */
