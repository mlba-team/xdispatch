#ifndef DISPATCH_WIN_TIMES_
#define DISPATCH_WIN_TIMES_

#include <stdlib.h>
#include <time.h>

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

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

// Definition of a gettimeofday function
static int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    // Define a structure to receive the current Windows filetime
    FILETIME ft;

    // Initialize the present time to 0 and the timezone to UTC
    ULARGE_INTEGER tmpres;
    //static int tzflag = 0;

    if (NULL != tv)
    {
        GetSystemTimeAsFileTime(&ft);

        // The GetSystemTimeAsFileTime returns the number of 100 nanosecond
        // intervals since Jan 1, 1601 in a structure.
        tmpres.LowPart  = ft.dwLowDateTime;
        tmpres.HighPart = ft.dwHighDateTime;        

        // Convert to microseconds by dividing by 10
        tmpres.QuadPart /= 10;

        // The Unix epoch starts on Jan 1 1970.  Need to subtract the difference
        // in seconds from Jan 1 1601.
        tmpres.QuadPart -= DELTA_EPOCH_IN_MICROSECS;

        // Finally change microseconds to seconds and place in the seconds value.
        // The modulus picks up the microseconds.
        tv->tv_sec = (long)(tmpres.QuadPart / 1000000UL);
        tv->tv_usec = (long)(tmpres.QuadPart % 1000000UL);
    }

    if (NULL != tz)
    {
        abort();
        /*
        if (!tzflag)
        {
            _tzset();
            tzflag++;
        }

        // Adjust for the timezone west of Greenwich
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
        */
    }

    return 0;
}

#endif /* DISPATCH_WIN_TIMES_ */
