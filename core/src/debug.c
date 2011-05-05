/*
* Copyright (c) 2008-2009 Apple Inc. All rights reserved.
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

#include "internal.h"

#if TARGET_OS_WIN32
 void * _ReturnAddress(void);
#endif

void
_dispatch_bug(size_t line, long val)
{
    static void *last_seen;
#ifdef __GNUC__
    void *ra = __builtin_return_address(0);
#elif _WIN32
    void *ra = _ReturnAddress();
#else
# error "Need return address"
#endif

    if (last_seen != ra) {
        last_seen = ra;
        _dispatch_log("BUG in libdispatch: %lu - 0x%lx", (unsigned long)line, val);
    }
}

void
_dispatch_abort(size_t line, long val)
{
    _dispatch_bug(line, val);
    abort();
}

void
_dispatch_log(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);

    _dispatch_logv(msg, ap);

    va_end(ap);
}

void _dispatch_logv(const char *msg, va_list params){
#if DISPATCH_DEBUG
    static FILE *logfile, *tmp;
# if TARGET_OS_WIN32
    char newbuf[300], temp[PATH_MAX];
# else
	char newbuf[strlen(msg) + 2];
# endif
    char path[PATH_MAX];

    sprintf(newbuf, "%s\n", msg);

    if (!logfile) {
# if TARGET_OS_WIN32
		ExpandEnvironmentStrings("%temp%\\libdispatch.%d.log", temp, PATH_MAX-2);
		snprintf(path, PATH_MAX, temp, getpid());
# else
        snprintf(path, sizeof(path), "/var/tmp/libdispatch.%d.log", getpid());
# endif
        tmp = fopen(path, "a");
        assert(tmp);
        if (!dispatch_atomic_ptr_cmpxchg(&logfile, NULL, tmp)) {
            fclose(tmp);
        } else {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            fprintf(logfile, "=== log file opened for %s[%u] at %ld.%06u ===\n",
                    getprogname() ?  getprogname() : "", getpid(), tv.tv_sec, (unsigned int)tv.tv_usec);
        }
    }
    vfprintf(logfile, newbuf, params);
    fflush(logfile);
#endif
}
