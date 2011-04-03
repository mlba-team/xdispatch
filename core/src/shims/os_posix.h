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

#ifndef SHIM_POSIX_H_
#define SHIM_POSIX_H_

#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <fcntl.h>

#include <signal.h>
#include <semaphore.h>
#include <limits.h>

#ifndef FD_COPY
#define	FD_COPY(f, t)	(void)(*(t) = *(f))
#endif

// really just a low level abort()
#define _dispatch_hardware_crash() __builtin_trap()

// some compiler hints
#define DISPATCH_NOINLINE	__attribute__((noinline))
#define DISPATCH_INLINE __attribute__((always_inline))
#define DISPATCH_UNUSED __attribute__((unused))
#define DISPATCH_NORETURN __attribute__((__noreturn__))

#endif /* SHIM_POSIX_H_ */
