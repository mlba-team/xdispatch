#ifndef _PTWQ_WINDOWS_PLATFORM_H
#define _PTWQ_WINDOWS_PLATFORM_H 1

#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string.h>

/* Instead of __attribute__ ((constructor)), use DllMain() */
#define CONSTRUCTOR	  /* */

#define VISIBLE __declspec(dllexport)

# define __func__ __FUNCTION__

#undef LIST_HEAD
#include "queue.h"

#define sleep(sec) 	Sleep(1000*sec)
#define strdup(p)	_strdup(p)
#define random()	rand()

typedef HANDLE pthread_t;

/* Emulation of pthreads mutex functionality */
/* (copied from libkqueue) */
#define PTHREAD_PROCESS_SHARED 1
#define PTHREAD_PROCESS_PRIVATE 2
typedef CRITICAL_SECTION pthread_mutex_t;
typedef CRITICAL_SECTION pthread_spinlock_t;
typedef CRITICAL_SECTION pthread_rwlock_t;
#define _cs_init(x)  InitializeCriticalSection((x))
#define _cs_lock(x)  EnterCriticalSection ((x))
#define _cs_unlock(x)  LeaveCriticalSection ((x))
#define pthread_mutex_lock _cs_lock
#define pthread_mutex_unlock _cs_unlock
#define pthread_mutex_init(x,y) _cs_init((x))
#define pthread_spin_lock _cs_lock
#define pthread_spin_unlock _cs_unlock
#define pthread_spin_init(x,y) _cs_init((x))
#define pthread_mutex_init(x,y) _cs_init((x))
#define pthread_rwlock_rdlock _cs_lock
#define pthread_rwlock_wrlock _cs_lock
#define pthread_rwlock_unlock _cs_unlock
#define pthread_rwlock_init(x,y) _cs_init((x))

#endif  /* _PTWQ_WINDOWS_PLATFORM_H */
