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



#ifndef DISPATCH_ATOMIC_H_
#define DISPATCH_ATOMIC_H_


/**
  Defines atomic operations in a platform independent way
  */

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2)

# if defined(__STDC__) && !defined(asm)
#  define asm __asm__
# endif

// GCC generates suboptimal register pressure
// LLVM does better, but doesn't support tail calls
// 6248590 __sync_*() intrinsics force a gratuitous "lea" instruction, with resulting register pressure
# if 0 && defined(__i386__) || defined(__x86_64__)
#  define dispatch_atomic_xchg(p, n)	({ typeof(*(p)) _r; asm("xchg %0, %1" : "=r" (_r) : "m" (*(p)), "0" (n)); _r; })
# else
#  define dispatch_atomic_xchg(p, n)	((typeof(*(p)))__sync_lock_test_and_set((p), (n)))
# endif
# define dispatch_atomic_cmpxchg(p, o, n)	__sync_bool_compare_and_swap((p), (o), (n))
# define dispatch_atomic_ptr_xchg(p, n) ((typeof(*(p)))__sync_lock_test_and_set((p), (n)))
# define dispatch_atomic_ptr_cmpxchg(p, o, n) dispatch_atomic_cmpxchg(p, o, n)
# define dispatch_atomic_inc(p)	__sync_add_and_fetch((p), 1)
# define dispatch_atomic_dec(p)	__sync_sub_and_fetch((p), 1)
# define dispatch_atomic_add(p, v)	__sync_add_and_fetch((p), (v))
# define dispatch_atomic_sub(p, v)	__sync_sub_and_fetch((p), (v))
# define dispatch_atomic_or(p, v)	__sync_fetch_and_or((p), (v))
# define dispatch_atomic_and(p, v)	__sync_fetch_and_and((p), (v))
# if defined(__i386__) || defined(__x86_64__)
/* GCC emits nothing for __sync_synchronize() on i386/x86_64. */
#  define dispatch_atomic_barrier()	__asm__ __volatile__("mfence")
# else
#  define dispatch_atomic_barrier()	__sync_synchronize()
# endif
# ifndef ATOMIC_INT
    typedef unsigned int ATOMIC_INT;
# endif

#elif defined _MSC_VER

# ifndef WINVER
#  define WINVER 0x0501
#  define WIN32_LEAN_AND_MEAN 1
#  include <Windows.h>
# endif

# include <intrin.h>

# pragma intrinsic(_InterlockedAnd)

# define dispatch_atomic_ptr_xchg(p, n) InterlockedExchangePointer((p),(n))
# define dispatch_atomic_ptr_cmpxchg(p, o, n) ( InterlockedCompareExchangePointer((p), (n), (o)) == (o) )
#if defined (_M_IA64) || defined (_M_X64) && !defined(__cplusplus)
# define dispatch_atomic_or(p, v)	InterlockedOr64((p), (v))
# define dispatch_atomic_inc(p)	InterlockedIncrement64((p))
# define dispatch_atomic_dec(p)	InterlockedDecrement64((p))
# define dispatch_atomic_cmpxchg(p, o, n)	( InterlockedCompareExchange64((p), (n), (o)) == (o) )
# define dispatch_atomic_xchg(p, n)	InterlockedExchange64((p),(n))
# define dispatch_atomic_add(p, v)	(InterlockedExchangeAdd64((p), (v)) + (v))
# define dispatch_atomic_sub(p, v)	(InterlockedExchangeAdd64((p), ((LONG)(v)*(-1))) - (v))
# define dispatch_atomic_and(p, v)	InterlockedAnd64((p), (v))
#else
# if defined(__cplusplus) && _MSC_VER <= 1500
#  define dispatch_atomic_or(p, v)	_InterlockedOr((p), (v))
#  define dispatch_atomic_inc(p)	InterlockedIncrement((LONG*)(p))
#  define dispatch_atomic_dec(p)	InterlockedDecrement((LONG*)(p))
#  define dispatch_atomic_cmpxchg(p, o, n)	( InterlockedCompareExchange((LONG*)(p), (n), (o)) == (o) )
#  define dispatch_atomic_xchg(p, n)	InterlockedExchange((LONG*)(p),(n))
#  define dispatch_atomic_add(p, v)	(InterlockedExchangeAdd((p), (v)) + (v))
#  define dispatch_atomic_sub(p, v)	(InterlockedExchangeAdd((p), ((LONG)(v)*(-1))) - (v))
#  define dispatch_atomic_and(p, v)	_InterlockedAnd((p), (v))
# else
#  define dispatch_atomic_or(p, v)	_InterlockedOr((p), (v))
#  define dispatch_atomic_inc(p)	InterlockedIncrement((p))
#  define dispatch_atomic_dec(p)	InterlockedDecrement((p))
#  define dispatch_atomic_cmpxchg(p, o, n)	( InterlockedCompareExchange((p), (n), (o)) == (o) )
#  define dispatch_atomic_xchg(p, n)	InterlockedExchange((p),(n))
#  define dispatch_atomic_add(p, v)	(InterlockedExchangeAdd((p), (v)) + (v))
#  define dispatch_atomic_sub(p, v)	(InterlockedExchangeAdd((p), ((LONG)(v)*(-1))) - (v))
#  define dispatch_atomic_and(p, v)	_InterlockedAnd((p), (v))
# endif
#endif
# define dispatch_atomic_barrier()	MemoryBarrier()

# ifndef ATOMIC_INT
#  if _MSC_VER >= 1600
    typedef unsigned int ATOMIC_INT;
#  else
    typedef long ATOMIC_INT;
#  endif
# endif

#else
# error "Compiler not supported."
#endif

#endif /* DISPATCH_ATOMIC_H_ */
