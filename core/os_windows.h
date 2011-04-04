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

#ifndef SHIM_WINDOWS_H_
#define SHIM_WINDOWS_H_

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
# define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

#include <Windows.h>
#if HAVE_STDINT_H
# include <stdint.h>
#else
# include "stdint.h"
#endif
#include <limits.h>

//typedef signed char int8_t;
//typedef unsigned char uint8_t;
//typedef signed int int16_t;
//typedef unsigned int uint16_t;
//typedef signed long int int32_t;
//typedef unsigned long int uint32_t;
typedef LONGLONG int64_t;
typedef UINT64 uint64_t;

#define inline __inline
#define bool BOOL

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

#define true 1
#define false 0

#ifndef FD_COPY
#define	FD_COPY(f, t)	(void)(*(t) = *(f))
#endif

// some date functions
struct timezone
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};

struct timespec {
    long tv_sec; /* seconds */
    long tv_nsec; /* nanoseconds */
}; 

int gettimeofday(struct timeval *tv, struct timezone *tz);

// some error codes used throughout the code
#define ENOTSUP 3
#define EAGAIN 4
#define EBADF 5
#define ETIMEDOUT 6
#define ESRCH 7
#define EINVAL 8

#endif /* SHIM_WINDOWS_H_ */
