/*
 * Copyright (c) 2011, Joakim Johansson <jocke@tbricks.com>
 *
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

#include <stdio.h>
#include <procfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <fcntl.h>

/* 
 
 /proc for Solaris
 
 STRUCTURE OF /proc/pid
 A given directory /proc/pid contains the following  entries.
 A  process  can  use  the  invisible  alias /proc/self if it
 wishes to open one of its own /proc files (invisible in  the
 sense  that the name ``self'' does not appear in a directory
 listing  of  /proc  obtained  from  ls(1),  getdents(2),  or
 readdir(3C)).
...
 lstatus
 Contains a  prheader  structure  followed  by  an  array  of
 lwpstatus structures, one for each active lwp in the process
 (see   also   /proc/pid/lwp/lwpid/lwpstatus,   below).   The
 prheader  structure  describes  the  number  and size of the
 array entries that follow.
 
 typedef struct prheader {
 long pr_nent;        // number of entries 
 size_t pr_entsize;   // size of each entry, in bytes 
 } prheader_t;

 The lwpstatus structure may grow by the addition of elements
 at  the  end in future releases of the system. Programs must
 use pr_entsize in the  file  header  to  index  through  the
 array.  These comments apply to all /proc files that include
 a prheader structure (lpsinfo and lusage, below).
 
 /proc/self/lstatus
 */

int threads_runnable(unsigned int *threads_running, unsigned int *threads_total)
{
    const char *path = "/proc/self/lstatus";
	int read_fd, retval = -1, i;
    unsigned int running_count = 0;
    char *lwp_buffer;
    ssize_t actual_read;
    lwpstatus_t *lwpstatus;
    prheader_t prheader;
	    
    read_fd = open(path, O_RDONLY);
	if (read_fd == -1) 
    {
        dbg_perror("open()");
        return retval;
	}

	if (fcntl(read_fd, F_SETFL, O_NONBLOCK) != 0) 
    {
        dbg_perror("fcntl()");
        goto errout;
	}

    actual_read = read(read_fd, &prheader, sizeof(prheader_t));

    if (actual_read != sizeof(prheader_t))
    {
        dbg_printf("read returned wrong number of bytes - %ld instead of %ld", (long int) actual_read, (long int) sizeof(prheader_t));
        goto errout;
    }

    dbg_printf("read prheader, pr_nent = %ld, pr_entsize = %ld, sizeof(lwpstatus_t) = %ld", (long int) prheader.pr_nent,  (long int) prheader.pr_entsize, (unsigned long) sizeof(lwpstatus_t));

    lwp_buffer = malloc(prheader.pr_nent * prheader.pr_entsize);

    if (!lwp_buffer)
    {
        dbg_perror("malloc(prheader.pr_nent * prheader.pr_entsize)");
        goto errout;
    }

    actual_read = read(read_fd, lwp_buffer, (prheader.pr_nent * prheader.pr_entsize));

    if (actual_read != (prheader.pr_nent * prheader.pr_entsize))
    {
        dbg_printf("read returned wrong number of bytes - %ld instead of %ld", (long) actual_read, (long) (prheader.pr_nent * prheader.pr_entsize));
        free(lwp_buffer);
        goto errout;
    }

    for (i = 0; i < prheader.pr_nent; i++)
    {
        lwpstatus = (lwpstatus_t *) (lwp_buffer + (i * prheader.pr_entsize));
        dbg_printf("lwp %d, syscall = %d", (int) lwpstatus->pr_lwpid, (int) lwpstatus->pr_syscall);
        
        if (lwpstatus->pr_flags & PR_ASLEEP)
        {            
            dbg_printf("lwp %d is sleeping", (int)lwpstatus->pr_lwpid);
        }   
        else
        {
            running_count++;
            dbg_printf("lwp %d is running", (int) lwpstatus->pr_lwpid);
        }        
    }

    free(lwp_buffer);
    retval = 0;
    *threads_running = running_count;
    *threads_total = prheader.pr_nent;
    
errout:
    if (close(read_fd) != 0)
    {
        dbg_perror("close()");
    }
    
    return retval;
}
