/*
 * Copyright (c) 2010 Apple Inc. All rights reserved.
 * Copyright (c) 2011-2013 MLBA-Team. All rights reserved.
 *
 * @APPLE_APACHE_LICENSE_HEADER_START@
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
 * @APPLE_APACHE_LICENSE_HEADER_END@
 */

#include "../internal.h"

#if !HAVE_PTHREAD_KEY_INIT_NP

pthread_key_t dispatch_queue_key;
pthread_key_t dispatch_sema4_key;
pthread_key_t dispatch_cache_key;
pthread_key_t dispatch_bcounter_key;

#endif

int _dispatch_pthread_sigmask(int how, sigset_t *set, sigset_t *oset)
{
#if !TARGET_OS_WIN32
    int r;

    /* Workaround: 6269619 Not all signals can be delivered on any thread */
    r = sigdelset(set, SIGILL);
    (void)dispatch_assume_zero(r);
    r = sigdelset(set, SIGTRAP);
    (void)dispatch_assume_zero(r);
#if HAVE_DECL_SIGEMT
    r = sigdelset(set, SIGEMT);
    (void)dispatch_assume_zero(r);
#endif
    r = sigdelset(set, SIGFPE);
    (void)dispatch_assume_zero(r);
    r = sigdelset(set, SIGBUS);
    (void)dispatch_assume_zero(r);
    r = sigdelset(set, SIGSEGV);
    (void)dispatch_assume_zero(r);
    r = sigdelset(set, SIGSYS);
    (void)dispatch_assume_zero(r);
    r = sigdelset(set, SIGPIPE);
    (void)dispatch_assume_zero(r);

    return pthread_sigmask(how, set, oset);
#else
	return 0;
#endif
}
