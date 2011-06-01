#ifndef DISPATCH_WIN_PTHREAD_CREATE_
#define DISPATCH_WIN_PTHREAD_CREATE_

/*
 * Posix Threads library for Microsoft Windows
 *
 * Use at own risk, there is no implied warranty to this code.
 * It uses undocumented features of Microsoft Windows that can change
 * at any time in the future.
 *
 * (C) 2010 Lockless Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of Lockless Inc. nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AN
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <setjmp.h>
#include <errno.h>

#ifndef ETIMEDOUT
# define ETIMEDOUT	110
#endif
#ifndef ENOTSUP
# define ENOTSUP		134
#endif


#define PTHREAD_CANCEL_DISABLE 0
#define PTHREAD_CANCEL_ENABLE 0x01

#define PTHREAD_CANCEL_DEFERRED 0
#define PTHREAD_CANCEL_ASYNCHRONOUS 0x02

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 0x04

#define PTHREAD_EXPLICT_SCHED 0
#define PTHREAD_INHERIT_SCHED 0x08

#define PTHREAD_SCOPE_PROCESS 0
#define PTHREAD_SCOPE_SYSTEM 0x10

#define PTHREAD_DEFAULT_ATTR (PTHREAD_CANCEL_ENABLE)

#define PTHREAD_CANCELED ((void *) 0xDEADBEEF)

#define PTHREAD_ONCE_INIT 0
#define PTHREAD_RWLOCK_INITIALIZER {0}
#define PTHREAD_COND_INITIALIZER {0}
#define PTHREAD_BARRIER_INITIALIZER \
	{0,0,PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER}
#define PTHREAD_SPINLOCK_INITIALIZER 0

#define PTHREAD_DESTRUCTOR_ITERATIONS 256
#define PTHREAD_KEYS_MAX (1<<20)

#define PTHREAD_PRIO_NONE 0
#define PTHREAD_PRIO_INHERIT 8
#define PTHREAD_PRIO_PROTECT 16
#ifndef PTHREAD_PRIO_MULT
# define PTHREAD_PRIO_MULT 32
#endif
#define PTHREAD_PROCESS_SHARED 0
#define PTHREAD_PROCESS_PRIVATE 1

#define PTHREAD_BARRIER_SERIAL_THREAD 1

typedef struct _pthread_cleanup _pthread_cleanup;
struct _pthread_cleanup
{
	void (*func)(void *);
	void *arg;
	_pthread_cleanup *next;
};

struct _pthread_v
{
	void *ret_arg;
	void *(* func)(void *);
	_pthread_cleanup *clean;
	HANDLE h;
	int cancelled;
	unsigned p_state;
	unsigned int keymax;
	void **keyval;
	
	jmp_buf jb;
};

typedef struct _pthread_v *pthread_t;

typedef struct pthread_attr_t pthread_attr_t;
struct pthread_attr_t
{
	unsigned p_state;
	void *stack;
	size_t s_size;
};

typedef long pthread_once_t;
typedef void *pthread_barrierattr_t;
typedef long pthread_spinlock_t;

#ifndef WIN_PTHREAD_EXPORT
#  ifdef DISPATCH_MAKEDLL
#   define WIN_PTHREAD_EXPORT __declspec(dllexport)
#  else
#   define WIN_PTHREAD_EXPORT __declspec(dllimport)
#  endif
#endif

/* These variables need to be declared somewhere,
   we recommend doing directly within the main file */
WIN_PTHREAD_EXPORT extern volatile long _pthread_cancelling;
WIN_PTHREAD_EXPORT extern int _pthread_concur;
/* Will default to zero as needed */
WIN_PTHREAD_EXPORT extern pthread_once_t _pthread_tls_once;
WIN_PTHREAD_EXPORT extern DWORD _pthread_tls;
WIN_PTHREAD_EXPORT extern pthread_mutex_t _pthread_key_lock;
WIN_PTHREAD_EXPORT extern unsigned int _pthread_key_max;
WIN_PTHREAD_EXPORT extern unsigned int _pthread_key_sch;
WIN_PTHREAD_EXPORT extern void (**_pthread_key_dest)(void *);

#define DECL_WIN_PTHREAD_VARIABLES \
	volatile long _pthread_cancelling; \
	int _pthread_concur; \
	pthread_once_t _pthread_tls_once = 0; \
	DWORD _pthread_tls; \
	pthread_mutex_t _pthread_key_lock = PTHREAD_MUTEX_INITIALIZER; \
	unsigned int _pthread_key_max; \
	unsigned _pthread_key_sch; \
	void (**_pthread_key_dest)(void *)

#define pthread_cleanup_push(F, A)\
{\
	const _pthread_cleanup _pthread_cup = {(F), (A), pthread_self()->clean};\
	_ReadWriteBarrier();\
	pthread_self()->clean = (_pthread_cleanup *) &_pthread_cup;\
	_ReadWriteBarrier()
	
/* Note that if async cancelling is used, then there is a race here */
#define pthread_cleanup_pop(E)\
	(pthread_self()->clean = _pthread_cup.next, (E?_pthread_cup.func(_pthread_cup.arg):0));}

static void _pthread_once_cleanup(void *o)
{
	*(pthread_once_t*)o = 0;
}

static pthread_t pthread_self(void);
static int pthread_once(pthread_once_t *o, void (*func)(void))
{
	long state = *o;

	_ReadWriteBarrier();
	
	while (state != 1)
	{
		if (!state)
		{
			if (!_InterlockedCompareExchange(o, 2, 0))
			{
				/* Success */
				pthread_cleanup_push(_pthread_once_cleanup, o);
				func();
				pthread_cleanup_pop(0);
				
				/* Mark as done */
				*o = 1;
				
				return 0;
			}
		}
		
		YieldProcessor();
		
		_ReadWriteBarrier();
		
		state = *o;
	}
	
	/* Done */
	return 0;
	
}

static int _pthread_once_raw(pthread_once_t *o, void (*func)(void))
{
	long state = *o;

	_ReadWriteBarrier();
	
	while (state != 1)
	{
		if (!state)
		{
			if (!_InterlockedCompareExchange(o, 2, 0))
			{
				/* Success */
				func();
				
				/* Mark as done */
				*o = 1;
				
				return 0;
			}
		}
		
		YieldProcessor();
		
		_ReadWriteBarrier();
		
		state = *o;
	}
	
	/* Done */
	return 0;
}

#define pthread_mutex_getprioceiling(M, P) ENOTSUP
#define pthread_mutex_setprioceiling(M, P) ENOTSUP

static int pthread_equal(pthread_t t1, pthread_t t2)
{
	return t1 == t2;
}


static void pthread_tls_init(void)
{
	_pthread_tls = TlsAlloc();
	
	/* Cannot continue if out of indexes */
	if (_pthread_tls == TLS_OUT_OF_INDEXES) abort();
}

static void _pthread_cleanup_dest(pthread_t t);

static pthread_t pthread_self(void)
{
	pthread_t t;
	
	_pthread_once_raw(&_pthread_tls_once, pthread_tls_init);
	
	t = (pthread_t)TlsGetValue(_pthread_tls);
	
	/* Main thread? */
	if (!t)
	{
		t = (pthread_t)malloc(sizeof(struct _pthread_v));
		
		/* If cannot initialize main thread, then the only thing we can do is abort */
		if (!t) abort();
	
		t->ret_arg = NULL;
		t->func = NULL;
		t->clean = NULL;
		t->cancelled = 0;
		t->p_state = PTHREAD_DEFAULT_ATTR;
		t->keymax = 0;
		t->keyval = NULL;
		t->h = GetCurrentThread();
		
		/* Save for later */
		TlsSetValue(_pthread_tls, t);
		
		if (setjmp(t->jb))
		{
			/* Make sure we free ourselves if we are detached */
			if (!t->h) free(t);
			
			/* Time to die */
			_endthreadex(0);
		}
	}
	
	return t;
}
	
static int pthread_getconcurrency(int *val)
{
	*val = _pthread_concur;
	return 0;
}

static int pthread_setconcurrency(int val)
{
	_pthread_concur = val;
	return 0;
}

#define pthread_getschedparam(T, P, S) ENOTSUP
#define pthread_setschedparam(T, P, S) ENOTSUP
#define pthread_getcpuclockid(T, C) ENOTSUP

static int pthread_exit(void *res)
{
	pthread_t t = pthread_self();

	t->ret_arg = res;
	
	_pthread_cleanup_dest(t);
	
	longjmp(t->jb, 1);
}


static void _pthread_invoke_cancel(void)
{
	_pthread_cleanup *pcup;
	
	_InterlockedDecrement(&_pthread_cancelling);
	
	/* Call cancel queue */
	for (pcup = pthread_self()->clean; pcup; pcup = pcup->next)
	{
		pcup->func(pcup->arg);
	}
		
	pthread_exit(PTHREAD_CANCELED);
}

static void pthread_testcancel(void)
{
	if (_pthread_cancelling)
	{
		pthread_t t = pthread_self();
		
		if (t->cancelled && (t->p_state & PTHREAD_CANCEL_ENABLE))
		{
			_pthread_invoke_cancel();
		}
	}
}


static int pthread_cancel(pthread_t t)
{
	if (t->p_state & PTHREAD_CANCEL_ASYNCHRONOUS)
	{
		/* Dangerous asynchronous cancelling */
		CONTEXT ctxt;
		
		/* Already done? */
		if (t->cancelled) return ESRCH;
		
		ctxt.ContextFlags = CONTEXT_CONTROL;
		
		SuspendThread(t->h);
		GetThreadContext(t->h, &ctxt);
#ifdef _M_X64
		ctxt.Rip = (uintptr_t) _pthread_invoke_cancel;
#else
		ctxt.Eip = (uintptr_t) _pthread_invoke_cancel;
#endif
		SetThreadContext(t->h, &ctxt);
		
		/* Also try deferred Cancelling */
		t->cancelled = 1;
	
		/* Notify everyone to look */
		_InterlockedIncrement(&_pthread_cancelling);
		
		ResumeThread(t->h);
	}
	else
	{
		/* Safe deferred Cancelling */
		t->cancelled = 1;
	
		/* Notify everyone to look */
		_InterlockedIncrement(&_pthread_cancelling);
	}
	
	return 0;
}

static unsigned _pthread_get_state(pthread_attr_t *attr, unsigned flag)
{
	return attr->p_state & flag;
}

static int _pthread_set_state(pthread_attr_t *attr, unsigned flag, unsigned val)
{
	if (~flag & val) return EINVAL;
	attr->p_state &= ~flag;
	attr->p_state |= val;
	
	return 0;
}

static int pthread_attr_init(pthread_attr_t *attr)
{
	attr->p_state = PTHREAD_DEFAULT_ATTR;
	attr->stack = NULL;
	attr->s_size = 0;
	return 0;
}

static int pthread_attr_destroy(pthread_attr_t *attr)
{
	/* No need to do anything */
	return 0;
}


static int pthread_attr_setdetachstate(pthread_attr_t *a, int flag)
{
	return _pthread_set_state(a, PTHREAD_CREATE_DETACHED, flag);
}

static int pthread_attr_getdetachstate(pthread_attr_t *a, int *flag)
{
	*flag = _pthread_get_state(a, PTHREAD_CREATE_DETACHED);
	return 0;
}

static int pthread_attr_setinheritsched(pthread_attr_t *a, int flag)
{
	return _pthread_set_state(a, PTHREAD_INHERIT_SCHED, flag);
}

static int pthread_attr_getinheritsched(pthread_attr_t *a, int *flag)
{
	*flag = _pthread_get_state(a, PTHREAD_INHERIT_SCHED);
	return 0;
}

static int pthread_attr_setscope(pthread_attr_t *a, int flag)
{
	return _pthread_set_state(a, PTHREAD_SCOPE_SYSTEM, flag);
}

static int pthread_attr_getscope(pthread_attr_t *a, int *flag)
{
	*flag = _pthread_get_state(a, PTHREAD_SCOPE_SYSTEM);
	return 0;
}

static int pthread_attr_getstackaddr(pthread_attr_t *attr, void **stack)
{
	*stack = attr->stack;
	return 0;
}

static int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stack)
{
	attr->stack = stack;
	return 0;
}

static int pthread_attr_getstacksize(pthread_attr_t *attr, size_t *size)
{
	*size = attr->s_size;
	return 0;
}

static int pthread_attr_setstacksize(pthread_attr_t *attr, size_t size)
{
	attr->s_size = size;
	return 0;
}

#define pthread_attr_getguardsize(A, S) ENOTSUP
#define pthread_attr_setgaurdsize(A, S) ENOTSUP
#define pthread_attr_getschedparam(A, S) ENOTSUP
#define pthread_attr_setschedparam(A, S) ENOTSUP
#define pthread_attr_getschedpolicy(A, S) ENOTSUP
#define pthread_attr_setschedpolicy(A, S) ENOTSUP


static int pthread_setcancelstate(int state, int *oldstate)
{
	pthread_t t = pthread_self();
	
	if ((state & PTHREAD_CANCEL_ENABLE) != state) return EINVAL;
	if (oldstate) *oldstate = t->p_state & PTHREAD_CANCEL_ENABLE;
	t->p_state &= ~PTHREAD_CANCEL_ENABLE;
	t->p_state |= state;
	
	return 0;
}

static int pthread_setcanceltype(int type, int *oldtype)
{
	pthread_t t = pthread_self();
	
	if ((type & PTHREAD_CANCEL_ASYNCHRONOUS) != type) return EINVAL;
	if (oldtype) *oldtype = t->p_state & PTHREAD_CANCEL_ASYNCHRONOUS;
	t->p_state &= ~PTHREAD_CANCEL_ASYNCHRONOUS;
	t->p_state |= type;
	
	return 0;
}

static unsigned int WINAPI pthread_create_wrapper(void *args)
{
	struct _pthread_v *tv = (struct _pthread_v *)args;
//	int i, j;
	
	_pthread_once_raw(&_pthread_tls_once, pthread_tls_init);
	
	TlsSetValue(_pthread_tls, tv);
		
	if (!setjmp(tv->jb))
	{
		/* Call function and save return value */
		tv->ret_arg = tv->func(tv->ret_arg);
		
		/* Clean up destructors */
		_pthread_cleanup_dest(tv);
	}
	
	/* If we exit too early, then we can race with create */
	while (tv->h == (HANDLE) -1)
	{
		YieldProcessor();
		_ReadWriteBarrier();
	}
	
	/* Make sure we free ourselves if we are detached */
	if (!tv->h) free(tv);
	
	return 0;
}

static int pthread_create(pthread_t *th, pthread_attr_t *attr, void *(* func)(void *), void *arg)
{
	struct _pthread_v *tv = (struct _pthread_v *)malloc(sizeof(struct _pthread_v));
	unsigned ssize = 0;
	
	if (!tv) return 1;
	
	*th = tv;
	
	/* Save data in pthread_t */
	tv->ret_arg = arg;
	tv->func = func;
	tv->clean = NULL;
	tv->cancelled = 0;
	tv->p_state = PTHREAD_DEFAULT_ATTR;
	tv->keymax = 0;
	tv->keyval = NULL;
	tv->h = (HANDLE) -1;
	
	if (attr)
	{
		tv->p_state = attr->p_state;
		ssize = attr->s_size;
	}
	
	/* Make sure tv->h has value of -1 */
	_ReadWriteBarrier();

	tv->h = (HANDLE) _beginthreadex(NULL, ssize, pthread_create_wrapper, tv, 0, NULL);
	
	/* Failed */
	if (!tv->h) return 1;
	
	
	if (tv->p_state & PTHREAD_CREATE_DETACHED)
	{
		CloseHandle(tv->h);
		_ReadWriteBarrier();
		tv->h = 0;
	}

	return 0;
}

static int pthread_join(pthread_t t, void **res)
{
	struct _pthread_v *tv = t;
	
	pthread_testcancel();
	
	WaitForSingleObject(tv->h, INFINITE);
	CloseHandle(tv->h);
	
	/* Obtain return value */
	if (res) *res = tv->ret_arg;
	
	free(tv);

	return 0;
}

static int pthread_detach(pthread_t t)
{
	struct _pthread_v *tv = t;
	
	/*
	 * This can't race with thread exit because
	 * our call would be undefined if called on a dead thread.
	 */
	
	CloseHandle(tv->h);
	_ReadWriteBarrier();
	tv->h = 0;
	
	return 0;
}

static int pthread_spin_init(pthread_spinlock_t *l, int pshared)
{
	(void) pshared;
	
	*l = 0;
	return 0;
}

static int pthread_spin_destroy(pthread_spinlock_t *l)
{
	(void) l;
	return 0;
}

/* No-fair spinlock due to lack of knowledge of thread number */
static int pthread_spin_lock(pthread_spinlock_t *l)
{
	while (_InterlockedExchange(l, EBUSY))
	{
		/* Don't lock the bus whilst waiting */
		while (*l)
		{
			YieldProcessor();
			
			/* Compiler barrier.  Prevent caching of *l */
			_ReadWriteBarrier();
		}
	}
	
	return 0;
}

static int pthread_spin_trylock(pthread_spinlock_t *l)
{
	return _InterlockedExchange(l, EBUSY);
}

static int pthread_spin_unlock(pthread_spinlock_t *l)
{
	/* Compiler barrier.  The store below acts with release symmantics */
	_ReadWriteBarrier();
	
	*l = 0;
	
	return 0;
}


/* No fork() in windows - so ignore this */
#define pthread_atfork(F1,F2,F3) 0

/* Windows has rudimentary signals support */
#define pthread_kill(T, S) 0
#define pthread_sigmask(H, S1, S2) 0

#endif /* DISPATCH_WIN_PTHREAD_CREATE_ */
