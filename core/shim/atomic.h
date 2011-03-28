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



#ifndef ATOMIC_H_
#define ATOMIC_H_

/**
  Defines atomic operations in a platform independent way
  */

#ifdef __GNUC__

#   ifdef WIN32_MINGW

    // on 32bit this is atomic by default
#	define atomic_inc_get(a) ++(*a)
#	define atomic_dec_get(a) --(*a)
#	define atomic_swap_get(a,b) __sync_val_compare_and_swap(a,*a, b)

#   else

#	define atomic_inc_get(a) __sync_add_and_fetch(a,1)
#	define atomic_dec_get(a) __sync_sub_and_fetch(a,1)
#	define atomic_swap_get(a,b) __sync_val_compare_and_swap(a,*a, b)

#   endif

#   define ATOMIC_INT unsigned int

#endif

#ifdef _MSC_VER

#	include <windows.h>

#		define atomic_inc_get(a) InterlockedIncrementAcquire(a)
#		define atomic_dec_get(a) InterlockedDecrementAcquire(a)
#		define atomic_swap_get(a,b) InterlockedExchange(a,b)

#if _MSC_VER >= 1600
#	define ATOMIC_INT unsigned int
#else
#	define ATOMIC_INT long
#endif

#endif

#endif /* ATOMIC_H_ */
