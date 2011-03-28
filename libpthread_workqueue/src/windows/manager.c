/*-
 * Copyright (c) 2011, Mark Heily <mark@heily.com>
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
#include "../private.h"
#include "pthread_workqueue.h"

static LIST_HEAD(, _pthread_workqueue) wqlist[WORKQ_NUM_PRIOQUEUE];
static CRITICAL_SECTION   wqlist_mtx;

int
manager_init(void)
{
    InitializeCriticalSection(&wqlist_mtx);
    return (0);
}

void
manager_workqueue_create(struct _pthread_workqueue *workq)
{
    EnterCriticalSection(&wqlist_mtx);
    LIST_INSERT_HEAD(&wqlist[workq->queueprio], workq, wqlist_entry);
    LeaveCriticalSection(&wqlist_mtx);
}

/* The caller must hold the wqlist_mtx. */
static struct work *
wqlist_scan(void)
{
    pthread_workqueue_t workq;
    struct work *witem = NULL;
    int i;

    EnterCriticalSection(&wqlist_mtx);
    for (i = 0; i < WORKQ_NUM_PRIOQUEUE; i++) {
        LIST_FOREACH(workq, &wqlist[i], wqlist_entry) {
            if (STAILQ_EMPTY(&workq->item_listhead))
            continue;

            witem = STAILQ_FIRST(&workq->item_listhead);
            if (witem != NULL)
                STAILQ_REMOVE_HEAD(&workq->item_listhead, item_entry);
            goto out;
        }
    }

out:
    LeaveCriticalSection(&wqlist_mtx);
    return (witem);
}

DWORD WINAPI
worker_main(LPVOID arg)
{
    struct work *witem;

    witem = wqlist_scan();
    if (witem == NULL)
	    return (0);

    witem->func(witem->func_arg);
    free(witem);
    return (0);
}

void
manager_workqueue_additem(struct _pthread_workqueue *workq, struct work *witem)
{
    EnterCriticalSection(&wqlist_mtx);
    STAILQ_INSERT_TAIL(&workq->item_listhead, witem, item_entry);
    LeaveCriticalSection(&wqlist_mtx);
    if (!QueueUserWorkItem(worker_main, NULL, WT_EXECUTELONGFUNCTION))
	    abort();
}
