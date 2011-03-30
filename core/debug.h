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


#ifndef DEBUG_H_
#define DEBUG_H_

void _dispatch_bug(size_t line, long val);
void _dispatch_abort(size_t line, long val);
void _dispatch_log(const char *msg, ...);

/*
 * For reporting bugs within libdispatch when using the "_debug" version of the library.
 */
#define dispatch_assert(e)	do {	\
        if (__builtin_constant_p(e)) {	\
            char __compile_time_assert__[(bool)(e) ? 1 : -1];	\
        } else {	\
            typeof(e) _e = fastpath(e); /* always eval 'e' */	\
            if (DISPATCH_DEBUG && !_e) {	\
                _dispatch_abort(__LINE__, (long)_e);	\
            }	\
        }	\
    } while (0)
/* A lot of API return zero upon success and not-zero on fail. Let's capture and log the non-zero value */
#define dispatch_assert_zero(e)	do {	\
        if (__builtin_constant_p(e)) {	\
            char __compile_time_assert__[(bool)(!(e)) ? 1 : -1];	\
        } else {	\
            typeof(e) _e = slowpath(e); /* always eval 'e' */	\
            if (DISPATCH_DEBUG && _e) {	\
                _dispatch_abort(__LINE__, (long)_e);	\
            }	\
        }	\
    } while (0)

/*
 * For reporting bugs or impedance mismatches between libdispatch and external subsystems.
 * These do NOT abort(), and are always compiled into the product.
 *
 * In particular, we wrap all system-calls with assume() macros.
 */
#define dispatch_assume(e)	({	\
        if (e==0) {	\
            if (__builtin_constant_p(e)) {	\
                char __compile_time_assert__[(e) ? 1 : -1];	\
                (void)__compile_time_assert__;	\
            }	\
            _dispatch_bug(__LINE__, (long)e);	\
        }	\
    })
/* A lot of API return zero upon success and not-zero on fail. Let's capture and log the non-zero value */
#define dispatch_assume_zero(e)	({	\
        if (e!=0) {	\
            if (__builtin_constant_p(e)) {	\
                char __compile_time_assert__[(e) ? 1 : -1];	\
                (void)__compile_time_assert__;	\
            }	\
            _dispatch_bug(__LINE__, (long)e);	\
        }	\
    })

/*
 * For reporting bugs in clients when using the "_debug" version of the library.
 */
#define dispatch_debug_assert(e, msg, args...)	do {	\
        if (__builtin_constant_p(e)) {	\
            char __compile_time_assert__[(bool)(e) ? 1 : -1];	\
        } else {	\
            typeof(e) _e = fastpath(e); /* always eval 'e' */	\
            if (DISPATCH_DEBUG && !_e) {	\
                _dispatch_log("%s() 0x%lx: " msg, __func__, (long)_e, ##args);	\
                abort();	\
            }	\
        }	\
    } while (0)

#endif /* DEBUG_H_ */
