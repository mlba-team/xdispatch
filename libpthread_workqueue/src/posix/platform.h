#ifndef _PTWQ_POSIX_PLATFORM_H
#define _PTWQ_POSIX_PLATFORM_H 1

/* Workaround to get visibility for _SC_NPROCESSORS_ONLN on FreeBSD */
#define __BSD_VISIBLE 1

#include <sys/resource.h>
#include <sys/queue.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>

#ifdef __sun
# include <sys/loadavg.h>
#endif

/* GCC atomic builtins. 
 * See: http://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Atomic-Builtins.html 
 */
#ifdef __sun
# include <atomic.h>
# define atomic_inc      atomic_inc_32
# define atomic_dec      atomic_dec_32
#else
# define atomic_inc(p)   __sync_add_and_fetch((p), 1)
# define atomic_dec(p)   __sync_sub_and_fetch((p), 1)
#endif

#ifdef MAKE_STATIC
# define CONSTRUCTOR
#else
# define CONSTRUCTOR __attribute__ ((constructor))
#endif
#define VISIBLE     __attribute__((visibility("default")))

/* 
 * Android does not provide spinlocks.
 * See: http://code.google.com/p/android/issues/detail?id=21622
 */
#if defined(__ANDROID_API__)
#define pthread_spinlock_t     pthread_mutex_t
#define pthread_spin_lock      pthread_mutex_lock
#define pthread_spin_unlock    pthread_mutex_unlock
#define pthread_spin_init(a,b) pthread_mutex_init((a), NULL)
#define pthread_spin_destroy   pthread_mutex_destroy
#endif /* defined(__ANDROID_API__) */

#endif  /* _PTWQ_POSIX_PLATFORM_H */
