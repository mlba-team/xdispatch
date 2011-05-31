#ifndef DISPATCH_WIN_PTHREAD_CREATE_
#define DISPATCH_WIN_PTHREAD_CREATE_

#include <process.h>

typedef HANDLE pthread_t;
typedef DWORD pthread_attr_t;

struct thread_param {
	void *(*start_routine)(void*);
	void* arg;
};

static unsigned int WINAPI thread_starter( LPVOID lpParam ) {
	struct thread_param* param = (struct thread_param*)(lpParam);
	param->start_routine(param->arg);
	free(param);

	return 0;
}

static int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
	void *(*start_routine)(void*), void *arg){
		/*
		HANDLE WINAPI CreateThread(
		__in_opt   LPSECURITY_ATTRIBUTES lpThreadAttributes,
		__in       SIZE_T dwStackSize,
		__in       LPTHREAD_START_ROUTINE lpStartAddress,
		__in_opt   LPVOID lpParameter,
		__in       DWORD dwCreationFlags,
		__out_opt  LPDWORD lpThreadId
		);
		uintptr_t _beginthreadex( // NATIVE CODE
		void *security,
		unsigned stack_size,
		unsigned ( __stdcall *start_address )( void * ),
		void *arglist,
		unsigned initflag,
		unsigned *thrdaddr
		);
		*/
		struct thread_param* param = (struct thread_param*) malloc(sizeof(struct thread_param));

		param->arg = arg;
		param->start_routine = start_routine;

		*thread = (HANDLE)_beginthreadex(0, 0, thread_starter, param, 0, 0 );
		// TODO: Proper error handling
		return *thread == 0;
}

static inline pthread_t pthread_self()
{
	return GetCurrentThread();
}

static void pthread_exit(int ret)
{
	_endthreadex(0);
}

static inline int pthread_join(pthread_t h, void** ret)
{
		return WaitForSingleObject(h, INFINITE);
}

static inline int pthread_detach(pthread_t t)
{
	return 0; 
}

#endif /* DISPATCH_WIN_PTHREAD_CREATE_ */
