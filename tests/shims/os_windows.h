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

#ifndef WINVER
# define WINVER 0x0501
#endif
#include <Windows.h>
#include <time.h>
#include <errno.h>

#if _MSC_VER < 1600
# include "../../core/platform/windows/stdint.h"
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
#if defined(_WIN32) && defined(DEBUG) && !defined(__GNUC__)
#	define _CRTDBG_MAP_ALLOC
#	include <stdlib.h>
#	include <crtdbg.h>
#endif

#define sleep(sec) 	Sleep(1000*sec)
#ifndef strdup
# define strdup(p)	_strdup(p)
#endif
#define random()	rand()


// missing error codes
#ifndef ETIMEDOUT
# define ETIMEDOUT WSAETIMEDOUT
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

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
# define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
# define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

// Definition of a gettimeofday function
static int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    // Define a structure to receive the current Windows filetime
    FILETIME ft;

    // Initialize the present time to 0 and the timezone to UTC
    unsigned __int64 tmpres = 0;
    static int tzflag = 0;

    if (NULL != tv)
    {
        GetSystemTimeAsFileTime(&ft);

        // The GetSystemTimeAsFileTime returns the number of 100 nanosecond
        // intervals since Jan 1, 1601 in a structure. Copy the high bits to
        // the 64 bit tmpres, shift it left by 32 then or in the low 32 bits.
        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        // Convert to microseconds by dividing by 10
        tmpres /= 10;

        // The Unix epoch starts on Jan 1 1970.  Need to subtract the difference
        // in seconds from Jan 1 1601.
        tmpres -= DELTA_EPOCH_IN_MICROSECS;

        // Finally change microseconds to seconds and place in the seconds value.
        // The modulus picks up the microseconds.
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (NULL != tz)
    {
        long timezone = 0;
        int daylight = 0;

        if (!tzflag)
        {
            _tzset();
            tzflag++;
        }

        // Adjust for the timezone west of Greenwich
#ifndef __GNUC__
        /* TODO: Currently this is not available on mingw, we should find a workaround for this */
        _get_timezone(&timezone);
        _get_daylight(&daylight);
#endif
        tz->tz_minuteswest = timezone / 60;        
        tz->tz_dsttime = daylight;
    }

    return 0;
}


#endif /* SHIM_WINDOWS_H_ */
