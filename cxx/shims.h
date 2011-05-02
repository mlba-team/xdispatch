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

#ifndef SHIMS_H_
#define SHIMS_H_

#include "../core/src/shims/atomic.h"

#ifdef _WIN32

/* Require Windows XP or later */
# define WINVER 0x0501
# define _WIN32_WINNT 0x0501

/* Reduces build time by omitting extra system headers */
# define WIN32_LEAN_AND_MEAN

# include <windows.h>

# include "../core/src/shims/os_windows.h"
# include "../core/src/shims/threads_windows.h"

typedef DWORD pthread_key_t;
# define pthread_key_create(k,d) (!((*k=TlsAlloc())!=TLS_OUT_OF_INDEXES))
# define pthread_setspecific(k,v) (!(TlsSetValue(k, v)))
# define pthread_getspecific(k) (TlsGetValue(k))

#else

# include <pthread.h>

#endif

#endif /* SHIMS_H_ */
