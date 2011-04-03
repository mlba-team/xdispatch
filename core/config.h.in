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



#ifndef CONFIG_H_
#define CONFIG_H_

//
// various configuration options
//
// WARNING! All changes made in config.h will be lost! Please edit config.h.in instead
//

// debug level
#ifndef DEBUG
# ifndef NDEBUG
#	define NDEBUG
# endif
# define DISPATCH_DEBUG 0
#else
# define DISPATCH_DEBUG 0
#endif

// define as 1 to enable the use of kevent
#define USE_KEVENTS 1

// mach kernel?
#ifdef __APPLE__
# define HAVE_MACH 1
#else
# define HAVE_MACH 0
#endif

// select the type of semaphore to use
#ifdef __APPLE__
# define USE_MACH_SEM 1
# define USE_POSIX_SEM 0
#else
# define USE_MACH_SEM 0
# define USE_POSIX_SEM 1
#endif

// we only implement the most current interface
#define DISPATCH_NO_LEGACY

// we always have pthreads_workqueue
#define HAVE_PTHREAD_WORKQUEUES 1

#ifdef __APPLE__
# define HAVE_PTHREAD_MAIN_NP 1
#else
# define HAVE_PTHREAD_MAIN_NP 0
#endif

// no progname ?
#define HAVE_GETPROGNAME

// no performance monitor
#define DISPATCH_PERF_MON 0

// type of kqueue implementation
#cmakedefine HAVE_KQUEUE_H 1
#cmakedefine HAVE_LIBKQUEUE_H 1

#endif /* CONFIG_H_ */
