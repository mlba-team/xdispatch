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
void _dispatch_logv(const char *msg, va_list);

#if HAVE_MACH
// MIG_REPLY_MISMATCH means either:
// 1) A signal handler is NOT using async-safe API. See the sigaction(2) man page for more info.
// 2) A hand crafted call to mach_msg*() screwed up. Use MIG.
#define DISPATCH_VERIFY_MIG(x) do {	\
        if ((x) == MIG_REPLY_MISMATCH) {	\
            _dispatch_hardware_crash();	\
        }	\
    } while (0)
#endif

#if __GNUC__

# if defined(__STDC__) && !defined(typeof)
#  define typeof __typeof__
# endif
/*
 * For reporting bugs within libdispatch when using the "_debug" version of the library.
 */
# define dispatch_assert(e)	do {	\
        if (__builtin_constant_p(e)) {	\
            char __compile_time_assert__[(bool)(e) ? 1 : -1] __attribute__((unused));	\
        } else {	\
            typeof(e) _e = fastpath(e); /* always eval 'e' */	\
            if (DISPATCH_DEBUG && !_e) {	\
                _dispatch_abort(__LINE__, (long)_e);	\
            }	\
        }	\
    } while (0)
/* A lot of API return zero upon success and not-zero on fail. Let's capture and log the non-zero value */
# define dispatch_assert_zero(e)	do {	\
        if (__builtin_constant_p(e)) {	\
            char __compile_time_assert__[(bool)(!(e)) ? 1 : -1] __attribute__((unused));	\
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
# define dispatch_assume(e)	({	\
        typeof(e) _e = fastpath(e); /* always eval 'e' */	\
        if (!_e) {	\
            if (__builtin_constant_p(e)) {	\
                char __compile_time_assert__[(e) ? 1 : -1];	\
                (void)__compile_time_assert__;	\
            }	\
            _dispatch_bug(__LINE__, (long)_e);	\
        }	\
        _e;	\
    })
/* A lot of API return zero upon success and not-zero on fail. Let's capture and log the non-zero value */
#define dispatch_assume_zero(e)	({	\
        typeof(e) _e = slowpath(e); /* always eval 'e' */	\
        if (_e) {	\
            if (__builtin_constant_p(e)) {	\
                char __compile_time_assert__[(e) ? -1 : 1];	\
                (void)__compile_time_assert__;	\
            }	\
            _dispatch_bug(__LINE__, (long)_e);	\
        }	\
        _e;	\
    })

/*
 * For reporting bugs in clients when using the "_debug" version of the library.
 */
# define dispatch_debug_assert(e, msg, args...)	do {	\
        if (__builtin_constant_p(e)) {	\
            char __compile_time_assert__[(bool)(e) ? 1 : -1] __attribute__((unused));	\
        } else {	\
            typeof(e) _e = fastpath(e); /* always eval 'e' */	\
            if (DISPATCH_DEBUG && !_e) {	\
                _dispatch_log("%s() 0x%lx: " msg, __func__, (long)_e, ##args);	\
                abort();	\
            }	\
        }	\
    } while (0)

/* Make sure the debug statments don't get too stale */
#define _dispatch_debug(x, args...)	\
    if (DISPATCH_DEBUG) {	\
        _dispatch_log("libdispatch: %u\t%p\t" x, __LINE__, (void *)_dispatch_thread_self(), ##args);	\
    }

#elif defined(TARGET_OS_WIN32)

/* MSVC can be really annoying as it does not allow us to specify
   a new variable within this block... */

# define dispatch_assert(e)	do {	\
        if (DISPATCH_DEBUG && !(e)) {	\
                _dispatch_abort(__LINE__, (long)0);	\
            }	\
    } while (0)

# define dispatch_assert_zero(e)	do {	\
        if (DISPATCH_DEBUG && (e)!=0) {	\
                _dispatch_abort(__LINE__, (long)0);	\
            }	\
    } while (0)

# define dispatch_assume(e)	\
        if ((e)==0) {	\
            _dispatch_bug(__LINE__, (long)0);	\
        }

# define dispatch_assume_zero(e) \
        if ((e)!=0) {	\
            _dispatch_bug(__LINE__, (long)0);	\
		}

# define dispatch_debug_assert(e, msg, ...)	do {	\
        if (DISPATCH_DEBUG && !(e)) {	\
                _dispatch_log("%s() 0x%lx: " msg, __func__, (long)0, ##args);	\
                abort();	\
        }	\
    } while (0)

#else

# define dispatch_assert(e)	do {	\
        long _e = (long)(e); \
        if (DISPATCH_DEBUG && !(_e)) {	\
                _dispatch_abort(__LINE__, (long)_e);	\
            }	\
    } while (0)

# define dispatch_assert_zero(e)	do {	\
        long _e = (long)(e); \
        if (DISPATCH_DEBUG && (_e)!=0) {	\
                _dispatch_abort(__LINE__, (long)_e);	\
            }	\
    } while (0)

# define dispatch_assume(e)	({	\
        long _e = (long)(e); \
        if (!(_e)) {	\
            _dispatch_bug(__LINE__, (long)_e);	\
        }	\
    })

# define dispatch_assume_zero(e)	({	\
        long _e = (long)(e); \
        if ((_e)!=0) {	\
            _dispatch_bug(__LINE__, (long)_e);	\
        }	\
    })

# define dispatch_debug_assert(e, msg, ...)	do {	\
        long _e = (long)(e); \
        if (DISPATCH_DEBUG && !(_e)) {	\
                _dispatch_log("%s() 0x%lx: " msg, __func__, (long)_e, ##args);	\
                abort();	\
        }	\
    } while (0)

/* Make sure the debug statments don't get too stale */
#define _dispatch_debug(x, ...)	\
    if (DISPATCH_DEBUG) {	\
        _dispatch_log("libdispatch: %u\t%p\t" x, __LINE__, (void *)_dispatch_thread_self(), ##args);	\
    }

#endif

#define DISPATCH_CRASH(x)	do {	\
        _dispatch_log("BUG IN LIBDISPATCH: %S ", x);	\
        _dispatch_hardware_crash();	\
    } while (0)

#define DISPATCH_CLIENT_CRASH(x)	do {	\
        _dispatch_log("BUG IN CLIENT OF LIBDISPATCH: %S ", x);	\
        _dispatch_hardware_crash();	\
    } while (0)

#endif /* DEBUG_H_ */
