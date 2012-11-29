#ifndef DARWIN_SEMAPHORE_H_
#define DARWIN_SEMAPHORE_H_

int sem_timedwait(sem_t * sem, const struct timespec * timeout);

#endif /* DARWIN_SEMAPHORE_H_ */