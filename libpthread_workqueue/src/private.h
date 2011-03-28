/*-
 * Copyright (c) 2011, Mark Heily <mark@heily.com>
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

#ifndef _PTWQ_PRIVATE_H
#define _PTWQ_PRIVATE_H 1

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
# include "windows/platform.h"
#else
# include "posix/platform.h"
#endif

#include "pthread_workqueue.h"
#include "debug.h"

/* The total number of priority levels. */
#define WORKQ_NUM_PRIOQUEUE 3

/* Signatures/magic numbers.  */
#define PTHREAD_WORKQUEUE_SIG       0xBEBEBEBE
#define PTHREAD_WORKQUEUE_ATTR_SIG  0xBEBEBEBE 

#if __GNUC__
#define fastpath(x)     ((__typeof__(x))__builtin_expect((long)(x), ~0l))
#define slowpath(x)     ((__typeof__(x))__builtin_expect((long)(x), 0l))
#else
#define fastpath(x) (x)
#define slowpath(x) (x)
#endif

#define CACHELINE_SIZE	64
#define ROUND_UP_TO_CACHELINE_SIZE(x)	(((x) + (CACHELINE_SIZE - 1)) & ~(CACHELINE_SIZE - 1))
#ifndef WITEM_CACHE_DISABLE
#   define WITEM_CACHE_DISABLE 1
#endif

struct work {
    STAILQ_ENTRY(work)   item_entry; 
    void               (*func)(void *);
    void                *func_arg;
    unsigned int         flags;
    unsigned int         gencount;
	struct work *volatile wi_next;
};

struct worker {
    LIST_ENTRY(worker)   entries;
    pthread_t            tid;
    enum {
        WORKER_STATE_SLEEPING,
        WORKER_STATE_RUNNING,
        WORKER_STATE_ZOMBIE,
    } state;
};

struct _pthread_workqueue {
    unsigned int         sig;    /* Unique signature for this structure */
    unsigned int         flags;
    int                  queueprio;
    int                  overcommit;
    LIST_ENTRY(_pthread_workqueue) wqlist_entry;
    STAILQ_HEAD(,work)   item_listhead;
    pthread_spinlock_t   mtx;
};

int manager_init(void);
void manager_workqueue_create(struct _pthread_workqueue *);
void manager_workqueue_additem(struct _pthread_workqueue *, struct work *);

struct work *witem_alloc_from_heap(void);
struct work *witem_alloc_cacheonly();
void witem_free(struct work *wi);
int witem_cache_init(void);
void witem_cache_cleanup(void *value);

#endif  /* _PTWQ_PRIVATE_H */

