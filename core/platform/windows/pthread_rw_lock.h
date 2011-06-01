#ifndef DISPATCH_WIN_PTHREAD_RWLOCK_
#define DISPATCH_WIN_PTHREAD_RWLOCK_

typedef CRITICAL_SECTION pthread_rwlock_t;
static int _cs_init(CRITICAL_SECTION* x){  InitializeCriticalSection(x); return 0; }
static int _cs_lock(CRITICAL_SECTION* x) {EnterCriticalSection (x); return 0; }
static int _cs_unlock(CRITICAL_SECTION* x) {LeaveCriticalSection(x); return 0; }
#define pthread_rwlock_rdlock _cs_lock
#define pthread_rwlock_wrlock _cs_lock
#define pthread_rwlock_unlock _cs_unlock
#define pthread_rwlock_init(x,y) _cs_init((x))

#endif /* DISPATCH_WIN_PTHREAD_RWLOCK_ */
