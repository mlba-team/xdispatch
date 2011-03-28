/*-
 * Copyright (c) 2010, Mark Heily <mark@heily.com>
 * Copyright (c) 2009, Stacey Son <sson@freebsd.org>
 * Copyright (c) 2000-2008, Apple Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "platform.h"
#include "private.h"
#include "pthread_workqueue.h"
#include "thread_info.h"
#include <sys/time.h>

/* Tunable constants */

#define WORKER_IDLE_SECONDS_THRESHOLD 5

/* Function prototypes */
static unsigned int get_load_average(void);
static void * worker_main(void *arg);
static unsigned int get_process_limit(void);
static void manager_start(void);

static unsigned int      cpu_count;

static LIST_HEAD(, worker) workers;
static unsigned int      worker_min;
static unsigned int      worker_idle_threshold; // we don't go down below this if we had to increase # workers

static LIST_HEAD(, _pthread_workqueue) wqlist[WORKQ_NUM_PRIOQUEUE];
static pthread_mutex_t   wqlist_mtx;

static pthread_cond_t    wqlist_has_work;
static int               wqlist_has_manager;
static pthread_cond_t    manager_init_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t   manager_mtx = PTHREAD_MUTEX_INITIALIZER;
static int               manager_has_initialized = 0;
static pthread_attr_t    detached_attr;

static struct {
    unsigned int    load,
                    count,
                    idle;
    unsigned int    sb_wake_pending;
    pthread_mutex_t sb_wake_mtx;
    pthread_cond_t  sb_wake_cond;
} scoreboard;

static unsigned int 
worker_idle_threshold_per_cpu()
{
    switch (cpu_count)
    {
        case 0:
        case 1:
        case 2:
        case 4:
          return 2;
        case 6:
          return 3;
        case 8:
        case 12:
          return 4;
        case 16:
        case 24:
          return 6;
        case 32:
        case 64:
          return 8;
        default:
            return cpu_count / 4;
    }
    
    return 2;
}

int
manager_init(void)
{
    int i;

    LIST_INIT(&workers);
    pthread_mutex_init(&wqlist_mtx, NULL);
    for (i = 0; i < WORKQ_NUM_PRIOQUEUE; i++)
        LIST_INIT(&wqlist[i]);

    witem_cache_init();

    cpu_count = (unsigned int) sysconf(_SC_NPROCESSORS_ONLN);

    pthread_attr_init(&detached_attr);
    pthread_attr_setdetachstate(&detached_attr, PTHREAD_CREATE_DETACHED);

    /* Initialize the scoreboard */
    pthread_cond_init(&scoreboard.sb_wake_cond, NULL);
    pthread_mutex_init(&scoreboard.sb_wake_mtx, NULL);

    /* Determine the initial thread pool constraints */
    worker_min = 2; // we can start with a small amount, worker_idle_threshold will be used as new dynamic low watermark
    worker_idle_threshold = worker_idle_threshold_per_cpu();

    manager_start();

    pthread_mutex_lock(&manager_mtx);
    
    while (!manager_has_initialized)
        pthread_cond_wait(&manager_init_cond, &manager_mtx);
    
    pthread_mutex_unlock(&manager_mtx);
    
    return (0);
}

void
manager_workqueue_create(struct _pthread_workqueue *workq)
{
    pthread_mutex_lock(&wqlist_mtx);
    LIST_INSERT_HEAD(&wqlist[workq->queueprio], workq, wqlist_entry);
    pthread_mutex_unlock(&wqlist_mtx);
}

/* The caller must hold the wqlist_mtx. */
static struct work *
wqlist_scan(void)
{
    pthread_workqueue_t workq;
    struct work *witem = NULL;
    int i;

    for (i = 0; i < WORKQ_NUM_PRIOQUEUE; i++) {
        LIST_FOREACH(workq, &wqlist[i], wqlist_entry) {
            if (STAILQ_EMPTY(&workq->item_listhead))
            continue;

            witem = STAILQ_FIRST(&workq->item_listhead);
            if (witem != NULL)
                STAILQ_REMOVE_HEAD(&workq->item_listhead, item_entry);
            pthread_mutex_unlock(&wqlist_mtx);
            goto out;
        }
    }

out:
    return (witem);
}

static void _wakeup_manager()
{
    dbg_puts("asking manager to wake up");

    pthread_mutex_lock(&scoreboard.sb_wake_mtx);
    scoreboard.sb_wake_pending = 1;
    pthread_cond_signal(&scoreboard.sb_wake_cond);
    pthread_mutex_unlock(&scoreboard.sb_wake_mtx);
    return;
}

static void *
worker_main(void *arg)
{
    struct worker *self = (struct worker *) arg;
    struct work *witem;
    void (*func)(void *);
    void *func_arg;

    dbg_puts("worker thread started");
    
    for (;;) {
        pthread_mutex_lock(&wqlist_mtx);

        self->state = WORKER_STATE_SLEEPING;

        while ((witem = wqlist_scan()) == NULL)
            pthread_cond_wait(&wqlist_has_work, &wqlist_mtx);

        self->state = WORKER_STATE_RUNNING;

        atomic_dec(&scoreboard.idle);

        if (slowpath(witem->func == NULL)) {
            dbg_puts("worker exiting..");
            self->state = WORKER_STATE_ZOMBIE;
            witem_free(witem);
            scoreboard.count--;
            pthread_exit(0);
        }

        dbg_printf("count=%u idle=%u wake_pending=%u", 
            scoreboard.count, scoreboard.idle,  scoreboard.sb_wake_pending);
        
        /* Force the manager thread to wakeup if all workers are busy */
        if (slowpath(scoreboard.idle == 0 && !scoreboard.sb_wake_pending))
            _wakeup_manager();

        /* Invoke the callback function, free witem first for possible reuse */
        func = witem->func;
        func_arg = witem->func_arg;
        witem_free(witem);
        
        func(func_arg);    
        
        atomic_inc(&scoreboard.idle); // initial inc was one in worker_start, this is to avoid a race
    }

    /* NOTREACHED */
    return (NULL);
}

static int
worker_start(void) 
{
    struct worker *wkr;

    dbg_puts("Spawning another worker");

    wkr = calloc(1, sizeof(*wkr));
    if (wkr == NULL) {
        dbg_perror("calloc(3)");
        return (-1);
    }

    scoreboard.count++;
    atomic_inc(&scoreboard.idle);

    if (pthread_create(&wkr->tid, &detached_attr, worker_main, wkr) != 0) {
        dbg_perror("pthread_create(3)");
        atomic_dec(&scoreboard.idle);
        scoreboard.count--;
        return (-1);
    }

    LIST_INSERT_HEAD(&workers, wkr, entries);

    return (0);
}

//static int
int
worker_stop(void) 
{
    struct work *witem;
    pthread_workqueue_t workq;
    int i;

    witem = fastpath(witem_alloc_cacheonly());
    if (!witem)
        witem = fastpath(witem_alloc_from_heap());
    memset(witem, 0, sizeof(*witem));

    pthread_mutex_lock(&wqlist_mtx);
    for (i = 0; i < WORKQ_NUM_PRIOQUEUE; i++) {
        workq = LIST_FIRST(&wqlist[i]);
        if (workq == NULL)
            continue;

        STAILQ_INSERT_TAIL(&workq->item_listhead, witem, item_entry);
        pthread_cond_signal(&wqlist_has_work);
        pthread_mutex_unlock(&wqlist_mtx);

        return (0);
    }

    /* FIXME: this means there are no workqueues.. should never happen */
    dbg_puts("Attempting to add a workitem without a workqueue");
    abort();

    return (-1);
}

static void *
manager_main(void *unused)
{
    //struct worker *wkr;
    unsigned int load_max = cpu_count;
    unsigned int worker_max, current_thread_count = 0;
    unsigned int worker_idle_seconds_accumulated = 0;
    unsigned int max_threads_to_stop = 0;
    int i, cond_wait_rv = 0;
    sigset_t sigmask, oldmask;
    struct timespec   ts;
    struct timeval    tp;

    worker_max = get_process_limit();
    scoreboard.load = get_load_average();

    /* Block all signals */
    sigfillset(&sigmask);
    pthread_sigmask(SIG_BLOCK, &sigmask, &oldmask);

    /* Create the minimum number of workers */
    scoreboard.count = 0;
    for (i = 0; i < worker_min; i++)
        worker_start();

    pthread_sigmask(SIG_SETMASK, &oldmask, NULL);

    pthread_mutex_lock(&manager_mtx);
    
    manager_has_initialized = 1;
    
    pthread_cond_signal(&manager_init_cond);
    pthread_mutex_unlock(&manager_mtx);

    for (;;) {

#if DEADWOOD
        /* Examine each worker and update statistics */
        scoreboard.sleeping = 0;
        scoreboard.running = 0;
        LIST_FOREACH(wkr, &workers, entries) {
            switch (wkr->state) {
                case WORKER_STATE_SLEEPING:
                    scoreboard.sleeping++;
                    break;

                case WORKER_STATE_RUNNING:
                    scoreboard.running++;
                    /* TODO: check for stalled worker */
                    break;

                case WORKER_STATE_ZOMBIE:
                    LIST_REMOVE(wkr, entries);
                    scoreboard.count--;
                    free(wkr);
                    break;
            }
        }
#endif

        pthread_mutex_lock(&scoreboard.sb_wake_mtx);

        dbg_puts("manager is sleeping");
        
        (void) gettimeofday(&tp, NULL); // TODO - error checking
        
        /* Convert from timeval to timespec */
        ts.tv_sec  = tp.tv_sec;
        ts.tv_nsec = tp.tv_usec * 1000;
        ts.tv_sec += 1; // wake once per second and check if we have too many idle threads...

        // We should only sleep on the condition if there are no pending signal, spurious wakeup is also ok
        if (scoreboard.sb_wake_pending == 0)
            cond_wait_rv = pthread_cond_timedwait(&scoreboard.sb_wake_cond, &scoreboard.sb_wake_mtx, &ts);

        scoreboard.sb_wake_pending = 0; // we must set this before spawning any new threads below, or we race...

        dbg_puts("manager is awake");

        dbg_printf("load=%u idle=%u workers=%u max_workers=%u worker_min = %u",
                   scoreboard.load, scoreboard.idle, scoreboard.count, worker_max, worker_min);
                
        // If no workers available, check if we should create a new one
        if (scoreboard.idle == 0 && (scoreboard.count > 0)) // last part required for an extremely unlikely race at startup
        {
            scoreboard.load = get_load_average();
            
            if ((scoreboard.load < load_max) && (scoreboard.count < worker_max)) 
            {
                if (scoreboard.count < worker_idle_threshold) // allow cheap rampup up to worker_idle_threshold without going to /proc
                {
                    worker_start();
                }
                else // check through /proc, will be a bit more expensive in terms of latency
                if (threads_runnable(&current_thread_count) == 0)
                {
                    // only start thread if we have less runnable threads than cpus
                    if (current_thread_count >= cpu_count)
                    {
                        dbg_printf("Not spawning worker thread, thread_runnable = %d >= cpu_count = %d", 
                                   current_thread_count, cpu_count);
                    }
                    else
                    {
                        worker_start();
                    }
                }
                else // always start thread if we can't get runnable count
                {
                    worker_start();
                }
            }
            else // high load, allow rampup up to worker_idle_threshold regardless of this
            {
                if (scoreboard.count < worker_idle_threshold) 
                {
                    worker_start();
                }                
            }
        }
        else
        {
            if (cond_wait_rv == ETIMEDOUT) // Only check for ramp down on the 'timer tick'
            {
                if ((scoreboard.idle - worker_idle_threshold) > 0) // only accumulate if there are 'too many' idle threads
                {
                    worker_idle_seconds_accumulated += scoreboard.idle; // keep track of many idle 'thread seconds' we have
                
                    dbg_printf("worker_idle_seconds_accumulated = %d, scoreboard.idle = %d, scoreboard.count = %d\n",
                       worker_idle_seconds_accumulated, scoreboard.idle, scoreboard.count);
                }
                
                // Only consider ramp down if we have accumulated enough thread 'idle seconds'
                // this logic will ensure that a large number of idle threads will ramp down faster
                max_threads_to_stop = worker_idle_seconds_accumulated / WORKER_IDLE_SECONDS_THRESHOLD;

                if (max_threads_to_stop > 0)
                {
                    worker_idle_seconds_accumulated = 0; 

                    if (max_threads_to_stop > (scoreboard.idle - worker_idle_threshold))
                        max_threads_to_stop = (scoreboard.idle - worker_idle_threshold);

                    // Only stop threads if we actually have 'too many' idle ones in the pool
                    if (scoreboard.idle > worker_idle_threshold)
                    {
                        for (i = 0; i < max_threads_to_stop; i++)
                        {
                            dbg_puts("Removing one thread from the thread pool");
                            worker_stop();
                        }                    
                    }
                }
            }            
        }
        
        pthread_mutex_unlock(&scoreboard.sb_wake_mtx);
    }

    /*NOTREACHED*/
    return (NULL);
}

static void
manager_start(void)
{
    pthread_t tid;
    int rv;

    dbg_puts("starting the manager thread");

    do {
        rv = pthread_create(&tid, &detached_attr, manager_main, NULL);
        if (rv == EAGAIN) {
            sleep(1);
        } else if (rv != 0) {
            /* FIXME: not nice */
            dbg_printf("thread creation failed, rv=%d", rv);
            abort();
        }
    } while (rv != 0);

    wqlist_has_manager = 1;
}

void
manager_workqueue_additem(struct _pthread_workqueue *workq, struct work *witem)
{
    /* TODO: possibly use a separate mutex or some kind of atomic CAS */
    pthread_mutex_lock(&wqlist_mtx);

    if (!wqlist_has_manager)
        manager_start();

    pthread_spin_lock(&workq->mtx);
    STAILQ_INSERT_TAIL(&workq->item_listhead, witem, item_entry);
    pthread_spin_unlock(&workq->mtx);

    pthread_cond_signal(&wqlist_has_work);
    pthread_mutex_unlock(&wqlist_mtx);
}


static unsigned int
get_process_limit(void)
{
#if __linux__
    struct rlimit rlim;

    if (getrlimit(RLIMIT_NPROC, &rlim) < 0) {
        dbg_perror("getrlimit(2)");
        return (50);
    } else {
        return (rlim.rlim_max);
    }
#else
    /* Solaris doesn't define this limit anywhere I can see.. */
    return (64);
#endif
}

static unsigned int
get_load_average(void)
{
    double loadavg;

    /* TODO: proper error handling */
    if (getloadavg(&loadavg, 1) != 1) {
        dbg_perror("getloadavg(3)");
        return (1);
    }
    if (loadavg > INT_MAX || loadavg < 0)
        loadavg = 1;

    return ((int) loadavg);
}
