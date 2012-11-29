
#include "semaphore.h"

int sem_timedwait(sem_t * sem, const struct timespec * timeout){
    int res;
    dispatch_time_t until = dispatch_walltime(timeout, 0);
    while( (res = sem_trywait(sem)) == EAGAIN
          && until < dispatch_time(DISPATCH_TIME_NOW,0))
        sleep(1);

    if (until >= dispatch_time(DISPATCH_TIME_NOW,0))
        return ETIMEDOUT;

    return res;
}
