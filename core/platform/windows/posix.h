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

#ifndef POSIX_WINDOWS_H_
#define POSIX_WINDOWS_H_

// Include Win32 headers early in order to minimize the
// likelihood of name pollution from dispatch headers.

#ifndef WINVER
# define WINVER 0x501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x502
#endif

//#define WIN32_LEAN_AND_MEAN 1
#ifndef _CRT_SECURE_NO_DEPRECATE
# define _CRT_SECURE_NO_DEPRECATE 1
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS 1
#endif
/* The #define doesn't seem to work, but the #pragma does.. */
#ifdef _MSC_VER
# pragma warning( disable : 4996 )
#endif

#include <Windows.h>
#include <stdint.h>
#include <limits.h>
#include <errno.h>
#include <process.h>

//typedef signed char int8_t;
//typedef unsigned char uint8_t;
//typedef signed int int16_t;
//typedef unsigned int uint16_t;
//typedef signed long int int32_t;
//typedef unsigned long int uint32_t;
typedef LONGLONG int64_t;
typedef UINT64 uint64_t;

#ifndef __cplusplus
# define inline __inline
# define bool BOOL
#endif /* __cplusplus */

// memory leak detection on windows
#if HAVE_CRTDBG_H && DISPATCH_DEBUG
#	define _CRTDBG_MAP_ALLOC
#	include <stdlib.h>
#	include <crtdbg.h>
#endif

#define sleep(sec) 	Sleep(1000*sec)
#ifndef strdup
# define strdup(p)	_strdup(p)
#endif
#ifndef snprintf
# define snprintf _snprintf
#endif
#define random()	rand()
#if !defined(__func__) && !defined(__GNUC__)
#define __func__ __FUNCDNAME__
#endif
#define getpid _getpid

#define true 1
#define false 0

#ifndef FD_COPY
#define	FD_COPY(f, t)	(void)(*(t) = *(f))
#endif

// some compiler hints
#ifndef DISPATCH_NOINLINE
# define DISPATCH_NOINLINE
#endif
#ifndef DISPATCH_INLINE
# define DISPATCH_INLINE
#endif
#ifndef DISPATCH_UNUSED
# define DISPATCH_UNUSED
#endif
#ifndef DISPATCH_NORETURN
# define DISPATCH_NORETURN
#endif

// some date functions
struct timezone
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};

// quick workaround in case this is included after event.h
#ifndef _SYS_EVENT_H_
struct timespec {
    long tv_sec; /* seconds */
    long tv_nsec; /* nanoseconds */
}; 
#endif

int gettimeofday(struct timeval *tv, struct timezone *tz);

// some constants used throughout the code
#ifndef ENOTSUP
# define ENOTSUP 300
#endif
#ifndef ETIMEDOUT
# define ETIMEDOUT WSAETIMEDOUT
#endif
#ifndef PATH_MAX
# define PATH_MAX 512
#endif

// fd functions
#include <io.h>
// TODO: Implement or workaround us
#define dup _dup
#define dup2 _dup2
#define close _close

#endif /* POSIX_WINDOWS_H_ */
