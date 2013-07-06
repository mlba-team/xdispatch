/*
 * Copyright (c) 2012, Joakim Johansson <jocke@tbricks.com>
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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <kstat.h>
#include <sys/sysinfo.h>

#include "../private.h"

static int previous_runque = 0, previous_updates = 0; // we assume we are only called by the manager thread

unsigned int
solaris_get_runqueue_length(void)
{
    kstat_ctl_t *kc;
    kstat_t *ksp;
    sysinfo_t sysinfo;
    unsigned int run_queue = 0;
    
    if (!(kc = kstat_open())){
        perror("kstat_open");
        return 0;
    }
    
    if (!(ksp = kstat_lookup(kc, "unix", 0, "sysinfo"))){
        perror("kstat_lookup");
        return 0;
    }
    
    if (kstat_read(kc, ksp, &sysinfo) == -1) {
        perror("kstat_read");
        return 0;
    }
    
    previous_runque = sysinfo.runque - previous_runque;
    previous_updates = sysinfo.updates - previous_updates;
    
    if (previous_updates != 0)
    {
        run_queue = (previous_runque/previous_updates);
        dbg_printf("runqueue = %u, updates = %u, ratio = %u", previous_runque, previous_updates, run_queue);
    }

    previous_runque = sysinfo.runque;
    previous_updates = sysinfo.updates;
    
    if (kstat_close(kc) != 0){
        perror("kstat_close");
        return 0;
    }

    return run_queue;
}
