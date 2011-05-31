#ifndef DISPATCH_WIN_PTHREAD_MUTEX_
#define DISPATCH_WIN_PTHREAD_MUTEX_

typedef CRITICAL_SECTION pthread_mutex_t;
typedef void pthread_mutex_attr_t;

static int pthread_mutex_lock(pthread_mutex_t* x)
{  
	EnterCriticalSection(x); 
	return 0; 
}

static int pthread_mutex_init(pthread_mutex_t* x, pthread_mutex_attr_t* y){
	InitializeCriticalSection(x);
	return 0;
}

static int pthread_mutex_unlock(pthread_mutex_t* x)
{  
	LeaveCriticalSection(x); 
	return 0; 
}

static int pthread_mutex_trylock(pthread_mutex_t* x)
{  
	EnterCriticalSection(x); 
	return 0; 
}

static int pthread_mutex_destroy(pthread_mutex_t* x)
{ 
	return 0; 
}

#define PTHREAD_MUTEX_INITIALIZER { 0 }

#endif /* DISPATCH_WIN_PTHREAD_MUTEX_ */
