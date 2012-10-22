/*
 * Copyright (c) 2008-2009 Apple Inc. All rights reserved.
 *
 * @APPLE_APACHE_LICENSE_HEADER_START@
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * @APPLE_APACHE_LICENSE_HEADER_END@
 */

#include <xdispatch/dispatch.h>

#include "tests.h"


#ifndef _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <spawn.h>
#include <signal.h>

#ifndef _WIN32
#	include <unistd.h>
#	include <sys/time.h>
#	include <sys/resource.h>
#	include <sys/wait.h>
#else
#	include <process.h>
#endif

//#include <libkern/OSAtomic.h>

#ifndef POSIX_SPAWN_START_SUSPENDED
# define POSIX_SPAWN_START_SUSPENDED 0
#endif

#define PID_CNT 5

static long event_cnt;

extern "C" void cxx_dispatch_proc_lambda(void)
{
    dispatch_source_t proc_native;
	int res;
	pid_t pid;

    MU_BEGIN_TEST(cxx_dispatch_proc_lambda);
	
	// Creates a process and register multiple observers.  Send a signal,
	// exit the process, etc., and verify all observers were notified.
	
	//
	// Simple child process that sleeps 2 seconds.
	//
	
	posix_spawnattr_t attr;
	res = posix_spawnattr_init(&attr);
    MU_ASSERT_TRUE( res );
	res = posix_spawnattr_setflags(&attr, POSIX_SPAWN_START_SUSPENDED);
    MU_ASSERT_TRUE( res );

	char* args[] = {
        (char*)"/bin/sleep", (char*)"2", NULL
	};
	
	res = posix_spawnp(&pid, args[0], NULL, &attr, args, NULL);
	if (res < 0) {
		perror(args[0]);
		exit(127);
	}

	res = posix_spawnattr_destroy(&attr);
    MU_ASSERT_TRUE( res );

    xdispatch::queue* completion = new xdispatch::queue("completion");
	
    MU_ASSERT_TRUE(pid > 0);

	//
	// Suspend the "completion" queue when each observer is created.
	// Each observer resumes the queue when the child process exits.
	// If the queue is resumed too few times (indicating that not all
	// observers received the exit event) then the test case will not exit
	// within the alloted time and result in failure.
	//
	
	int i;
	for (i = 0; i < PID_CNT; ++i) {
        completion->suspend();
        proc_native = dispatch_source_create(DISPATCH_SOURCE_TYPE_PROC, pid, DISPATCH_PROC_EXIT, dispatch_get_main_queue());
        xdispatch::source* proc = new xdispatch::source( new xdispatch::native_source(proc_native) );
        MU_DESC_ASSERT_NOT_NULL("DISPATCH_SOURCE_TYPE_PROC", proc);

        proc->handler([=]{
            long flags = dispatch_source_get_data( proc->native_source() );
            MU_DESC_ASSERT_EQUAL("DISPATCH_PROC_EXIT", flags, DISPATCH_PROC_EXIT);
			event_cnt++;
            completion->resume();
		});

        proc->resume();
	}


	//
	// The completion block will be pending on the completion queue until it
	// has been fully resumed, at which point the test will exit successfully.
	//

    completion->async([=]{
		int status;
		int res2 = waitpid(pid, &status, 0);
        MU_ASSERT_TRUE(res2 != -1);
        MU_DESC_ASSERT_EQUAL("Sub-process exited", WEXITSTATUS(status) | WTERMSIG(status), 0);
        MU_DESC_ASSERT_EQUAL("Event count", event_cnt, PID_CNT);
        MU_PASS("");
	});

	kill(pid, SIGCONT);

    xdispatch::exec();

    MU_FAIL("Should never reach this");
    MU_END_TEST;
}


#endif /* _WIN32 */
