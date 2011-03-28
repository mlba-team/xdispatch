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

#include "private.h"

int DEBUG = 0;
char *DEBUG_IDENT = "WQ";

static int
valid_workq(pthread_workqueue_t workq) 
{
    if (workq->sig == PTHREAD_WORKQUEUE_SIG)
        return (1);
    else
        return (0);
}

int VISIBLE CONSTRUCTOR
pthread_workqueue_init_np(void)
{
#ifdef NDEBUG
    DEBUG = 0;
#elif _WIN32
	/* Experimental port, always debug */
	DEBUG = 1;
#else
    DEBUG = (getenv("PWQ_DEBUG") == NULL) ? 0 : 1;
#endif

    if (manager_init() < 0)
        return (-1);
 
    dbg_puts("pthread_workqueue library initialized");
    return (0);
}

int VISIBLE
pthread_workqueue_create_np(pthread_workqueue_t *workqp,
                            const pthread_workqueue_attr_t * attr)
{
    pthread_workqueue_t workq;

    if ((attr != NULL) && ((attr->sig != PTHREAD_WORKQUEUE_ATTR_SIG) ||
         (attr->queueprio < 0) || (attr->queueprio > WORKQ_NUM_PRIOQUEUE)))
        return (EINVAL);
    if ((workq = calloc(1, sizeof(*workq))) == NULL)
        return (ENOMEM);
    workq->sig = PTHREAD_WORKQUEUE_SIG;
    workq->flags = 0;
    STAILQ_INIT(&workq->item_listhead);
    pthread_spin_init(&workq->mtx, PTHREAD_PROCESS_PRIVATE);
    if (attr == NULL) {
        workq->queueprio = WORKQ_DEFAULT_PRIOQUEUE;
        workq->overcommit = 0;
    } else {
        workq->queueprio = attr->queueprio;
        workq->overcommit = attr->overcommit;
    }

    manager_workqueue_create(workq);

    dbg_printf("created queue %p", workq);

    *workqp = workq;
    return (0);
}

int VISIBLE
pthread_workqueue_additem_np(pthread_workqueue_t workq,
                     void (*workitem_func)(void *), void * workitem_arg,
                     pthread_workitem_handle_t * itemhandlep, 
                     unsigned int *gencountp)
{
    struct work *witem;
    
    if (valid_workq(workq) == 0)
        return (EINVAL);

    witem = fastpath(witem_alloc_cacheonly());
    if (slowpath(witem == NULL))
        witem = witem_alloc_from_heap();

    witem->gencount = 0;
    witem->func = workitem_func;
    witem->func_arg = workitem_arg;
    witem->flags = 0;
    witem->item_entry.stqe_next = 0;

    manager_workqueue_additem(workq, witem);

    if (itemhandlep != NULL)
        *itemhandlep = (pthread_workitem_handle_t *) witem;
    if (gencountp != NULL)
        *gencountp = witem->gencount;

    dbg_printf("added an item to queue %p", workq);

    return (0);
}

int VISIBLE
pthread_workqueue_attr_init_np(pthread_workqueue_attr_t *attr)
{
    attr->queueprio = WORKQ_DEFAULT_PRIOQUEUE;
    attr->sig = PTHREAD_WORKQUEUE_ATTR_SIG;
    attr->overcommit = 0;
    return (0);
}

int VISIBLE
pthread_workqueue_attr_destroy_np(pthread_workqueue_attr_t *attr)
{
    if (attr->sig == PTHREAD_WORKQUEUE_ATTR_SIG)
        return (0);
    else
        return (EINVAL); /* Not an attribute struct. */
}

int VISIBLE
pthread_workqueue_attr_getovercommit_np(
        const pthread_workqueue_attr_t *attr, int *ocommp)
{
    if (attr->sig == PTHREAD_WORKQUEUE_ATTR_SIG) {
        *ocommp = attr->overcommit;
        return (0);
    } else 
        return (EINVAL); /* Not an attribute struct. */
}

int VISIBLE
pthread_workqueue_attr_setovercommit_np(pthread_workqueue_attr_t *attr,
                           int ocomm)
{
    if (attr->sig == PTHREAD_WORKQUEUE_ATTR_SIG) {
        attr->overcommit = ocomm;
        return (0);
    } else
        return (EINVAL);
}

int VISIBLE
pthread_workqueue_attr_getqueuepriority_np(
        pthread_workqueue_attr_t *attr, int *qpriop)
{
    if (attr->sig == PTHREAD_WORKQUEUE_ATTR_SIG) {
        *qpriop = attr->queueprio;
        return (0);
    } else 
        return (EINVAL);
}

int VISIBLE
pthread_workqueue_attr_setqueuepriority_np(
        pthread_workqueue_attr_t *attr, int qprio)
{
    if (attr->sig == PTHREAD_WORKQUEUE_ATTR_SIG) {
        switch(qprio) {
            case WORKQ_HIGH_PRIOQUEUE:
            case WORKQ_DEFAULT_PRIOQUEUE:
            case WORKQ_LOW_PRIOQUEUE:
                attr->queueprio = qprio;
                return (0);
            default:
                return (EINVAL);
        }
    } else
        return (EINVAL);
}

/*
 * Does not exist in the Apple implementation, but needed on Linux
 * due to a kernel bug that causes the process to become a zombie when
 * the main thread calls pthread_exit().
 *
 * More info:
 *   http://www.0x61.com/forum/viewtopic.php?f=109&t=997736&view=next
 */
void VISIBLE
pthread_workqueue_main_np(void)
{

    //TESTING - dispatch testsuite requires this..
#ifndef _WIN32
	pthread_exit(0);
#endif

    /* 
    struct worker w;
    pthread_mutex_lock(&wqlist_mtx);
    if (wqlist_has_manager) {
        pthread_mutex_unlock(&wqlist_mtx);
        dbg_puts("running as a worker");
        memset(&w, 0, sizeof(w));
        worker_main(&w);
    } else {
        wqlist_has_manager = 1;
        pthread_mutex_unlock(&wqlist_mtx);

        dbg_puts("running as a manager");
        manager_main(NULL);
    }
    */
}
