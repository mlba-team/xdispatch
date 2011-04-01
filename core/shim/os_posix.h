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

#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <malloc/malloc.h>
#include <sys/event.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <signal.h>
#include <semaphore.h>

#if HAVE_MACH
# include <mach/boolean.h>
# include <mach/clock_types.h>
# include <mach/clock.h>
# include <mach/exception.h>
# include <mach/mach.h>
# include <mach/mach_error.h>
# include <mach/mach_host.h>
# include <mach/mach_interface.h>
# include <mach/mach_time.h>
# include <mach/mach_traps.h>
# include <mach/message.h>
# include <mach/mig_errors.h>
# include <mach/host_info.h>
# include <mach/notify.h>
#endif

// really just a low level abort()
#define _dispatch_hardware_crash() __builtin_trap()

// some compiler hints
#define DISPATCH_NOINLINE	__attribute__((noinline))
#define DISPATCH_INLINE __attribute__((always_inline))
#define DISPATCH_UNUSED __attribute__((unused))
#define DISPATCH_NORETURN __attribute__((__noreturn__))

#endif /* SHIM_POSIX_H_ */
