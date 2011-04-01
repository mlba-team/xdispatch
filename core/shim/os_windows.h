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

#define WINVER 0x0501
#include <Windows.h>
#include <Error.h>

#if _MSC_VER < 1600
# include "stdint.h"
#else
# include <stdint.h>
#endif
//typedef signed char int8_t;
//typedef unsigned char uint8_t;
//typedef signed int int16_t;
//typedef unsigned int uint16_t;
//typedef signed long int int32_t;
//typedef unsigned long int uint32_t;
typedef LONGLONG int64_t;
typedef UINT64 uint64_t;

#define inline __inline

// memory leak detection on windows
#if defined(_WIN32) && defined(DEBUG)
#	define _CRTDBG_MAP_ALLOC
#	include <stdlib.h>
#	include <crtdbg.h>
#endif

#define sleep(sec) 	Sleep(1000*sec)
#ifndef strdup
# define strdup(p)	_strdup(p)
#endif
#define random()	rand()

// really just a low level abort()
#define _dispatch_hardware_crash() __debugbreak()

// some compiler hints
#define DISPATCH_NOINLINE
#define DISPATCH_UNUSED
#define DISPATCH_NORETURN

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

#endif /* SHIM_WINDOWS_H_ */
