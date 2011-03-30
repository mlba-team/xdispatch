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

#define _CRT_SECURE_NO_WARNINGS
#include "internal.h"

#ifdef _WIN32
 void * _ReturnAddress(void);
#endif

void
_dispatch_bug(size_t line, long val)
{
    static void *last_seen;
#ifdef _WIN32
    void *ra = _ReturnAddress();
#else
    void *ra = __builtin_return_address(0);
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
_dispatch_log(const char *format, ...)
{
#ifdef DISPATCH_DEBUG
    va_list params;
    char tmp[201];
    va_start(params, format);
    vsprintf(tmp, format, params);
    va_end(params);
    printf("%s\n",tmp);
    fflush(stdout);
#endif
}

