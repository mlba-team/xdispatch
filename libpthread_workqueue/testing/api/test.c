#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if !defined(_WIN32)
# if !defined(NO_CONFIG_H)
#  include "config.h"
# endif
#else
# define sleep(t) Sleep(1000*t)
#endif
#include "src/private.h"

#if HAVE_ERR_H
# include <err.h>
#else
# define err(rc,msg,...) do { perror(msg); exit(rc); } while (0)
# define errx(rc,msg,...) do { puts(msg); exit(rc); } while (0)
#endif

#include "pthread_workqueue.h"

static int work_cnt;

pthread_workqueue_t wq;

void additem(void (*func)(void *), 
             void * arg)
{

    int rv;
    
    rv = pthread_workqueue_additem_np(wq, *func, arg, NULL, NULL);
    if (rv != 0)
        errx(1, "unable to add item: %s", strerror(rv));
    puts("added item\n");
}

#define nval 5000

void
compute(void *arg)
{
    int *count = (int *) arg;
    int val[nval];
    int i,j;

    /* Do some useless computation */
    for (i = 0; i < nval; i++) {
        val[i] = INT_MAX;
    }
    for (j = 0; j < nval; j++) {
        for (i = 0; i < nval; i++) {
            val[i] /= 3;
            val[i] *= 2;
            val[i] /= 4;
            val[i] *= 5;
        }
    }

    if (count != NULL) 
        (*count)--;
}


void
sleepy(void *msg)
{
    printf("%s\n", (char *) msg);
    if (strcmp(msg, "done") == 0)
        exit(0);
    sleep(random() % 6);
}

void
lazy(void *arg)
{
    sleep(3);
    printf("item %lu complete\n", (unsigned long) arg);
	work_cnt--;
}

void
run_blocking_test(void)
{
	const int rounds = 50;
	int i = 0;
	work_cnt = rounds;
    for (i = 0; i < rounds; i++) {
        additem(lazy, (void *) i);
    }
	while (work_cnt > 0)
		sleep(1);
}

void
run_cond_wait_test(void)
{
	const int rounds = 10;
	int i = 0;

	sleep(3);	/* Allow time for the workers to enter pthread_cond_wait() */
	work_cnt = rounds;
    for (i = 0; i < rounds; i++) {
        additem(lazy, (void *) i);
		sleep(1);
    }
	while (work_cnt > 0)
		sleep(1);
}

void
run_load_test(void)
{
    char buf[16];
	int i = 0;
    for (i = 0; i < 1024; i++) {
        sprintf(buf, "%d", i);
        additem(sleepy, strdup(buf));
        additem(compute, NULL);
    }
    additem(sleepy, "done");
}

/* Try to overwhelm the CPU with computation requests */
void
run_stress_test(int rounds)
{
	int i = 0;
	work_cnt = rounds;
    for (i = 0; i < rounds; i++) {
        additem(compute, &work_cnt);
    }
	while (work_cnt > 0)
		sleep(1);
    puts("====== stress test complete =======");
}


int main() {
    int rv;

    printf("pthread_workqueue_create_np().. ");
    rv = pthread_workqueue_create_np(&wq, NULL);
    if (rv != 0)
        err(1, "failed");
    printf("ok\n");

    printf("stress test.. ");
    run_stress_test(25);
    printf("ok\n");

    //run_deadlock_test();
//    run_cond_wait_test();
//    run_blocking_test();
    //run_load_test();


	puts("All tests completed.\n");
    exit(0);
}
