#ifndef DISPATCH_WIN_PTHREAD_TLS_
#define DISPATCH_WIN_PTHREAD_TLS_

typedef DWORD pthread_key_t;
#define pthread_key_create(k,d) (!((*k=TlsAlloc())!=TLS_OUT_OF_INDEXES))
#define pthread_setspecific(k,v) (!(TlsSetValue(k, v)))
#define pthread_getspecific(k) (TlsGetValue(k))

#endif /* DISPATCH_WIN_PTHREAD_TLS_ */
