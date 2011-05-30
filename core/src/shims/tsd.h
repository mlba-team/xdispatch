
/*
 * IMPORTANT: This header file describes INTERNAL interfaces to libdispatch
 * which are subject to change in future releases of Mac OS X. Any applications
 * relying on these interfaces WILL break.
 */

#ifndef __DISPATCH_SHIMS_TSD__
#define __DISPATCH_SHIMS_TSD__

#ifdef __APPLE__
int sem_timedwait(sem_t *, const struct timespec *);
#endif

#if defined(__STDC__) && !defined(inline)
# define inline __inline__
#endif

#if HAVE_PTHREAD_KEY_INIT_NP
static const unsigned long dispatch_queue_key = __PTK_LIBDISPATCH_KEY0;
static const unsigned long dispatch_sema4_key = __PTK_LIBDISPATCH_KEY1;
static const unsigned long dispatch_cache_key = __PTK_LIBDISPATCH_KEY2;
static const unsigned long dispatch_bcounter_key = __PTK_LIBDISPATCH_KEY3;
//__PTK_LIBDISPATCH_KEY4
//__PTK_LIBDISPATCH_KEY5
#else
extern pthread_key_t dispatch_queue_key;
extern pthread_key_t dispatch_sema4_key;
extern pthread_key_t dispatch_cache_key;
extern pthread_key_t dispatch_bcounter_key;
#endif

#if USE_APPLE_TSD_OPTIMIZATIONS
# define SIMULATE_5491082 1
# ifndef _PTHREAD_TSD_OFFSET
#  define _PTHREAD_TSD_OFFSET 0
# endif

static inline void
_dispatch_thread_setspecific(unsigned long k, void *v)
{
#if defined(SIMULATE_5491082) && defined(__i386__)
    asm("movl %1, %%gs:%0" : "=m" (*(void **)(k * sizeof(void *) + _PTHREAD_TSD_OFFSET)) : "ri" (v) : "memory");
#elif defined(SIMULATE_5491082) && defined(__x86_64__)
    asm("movq %1, %%gs:%0" : "=m" (*(void **)(k * sizeof(void *) + _PTHREAD_TSD_OFFSET)) : "rn" (v) : "memory");
#else
    int res;
    if (_pthread_has_direct_tsd()) {
        res = _pthread_setspecific_direct(k, v);
    } else {
        res = pthread_setspecific(k, v);
    }
    dispatch_assert_zero(res);
#endif
}

static inline void *
_dispatch_thread_getspecific(unsigned long k)
{
#if defined(SIMULATE_5491082) && (defined(__i386__) || defined(__x86_64__))
    void *rval;
    asm("mov %%gs:%1, %0" : "=r" (rval) : "m" (*(void **)(k * sizeof(void *) + _PTHREAD_TSD_OFFSET)));
    return rval;
#else
    if (_pthread_has_direct_tsd()) {
        return _pthread_getspecific_direct(k);
    } else {
        return pthread_getspecific(k);
    }
#endif
}

#else /* !USE_APPLE_TSD_OPTIMIZATIONS */

static inline void
_dispatch_thread_setspecific(pthread_key_t k, void *v)
{
    int res;

    res = pthread_setspecific(k, v);
    dispatch_assert_zero(res);
}

static inline void *
_dispatch_thread_getspecific(pthread_key_t k)
{

    return pthread_getspecific(k);
}
#endif /* USE_APPLE_TSD_OPTIMIZATIONS */

#if HAVE_PTHREAD_KEY_INIT_NP
static inline void
_dispatch_thread_key_init_np(unsigned long k, void (*d)(void *))
{
    dispatch_assert_zero(pthread_key_init_np((int)k, d));
}
#else
static inline void
_dispatch_thread_key_create(pthread_key_t *key, void (*destructor)(void *))
{

    dispatch_assert_zero(pthread_key_create(key, destructor));
}
#endif

#define _dispatch_thread_self (uintptr_t)pthread_self

int _dispatch_pthread_sigmask(int how, sigset_t *, sigset_t *);

#endif /* __DISPATCH_SHIMS_TSD__ */
