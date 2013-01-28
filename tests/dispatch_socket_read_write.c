/*
* Copyright (c) 2012-2013 MLBA-Team. All rights reserved.
*
* @MLBA_OPEN_LICENSE_HEADER_START@
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
* @MLBA_OPEN_LICENSE_HEADER_END@
*/

#include <xdispatch/dispatch.h>

#include "tests.h"

#ifdef _WIN32
#else
# include <sys/stat.h>
# include <assert.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <errno.h>
# include <sys/types.h>
# include <sys/socket.h>
#endif


#if DISPATCH_SOURCE_HAS_READ && DISPATCH_SOURCE_HAS_WRITE

static int sock_fd;
static dispatch_queue_t sock_queue;
static dispatch_source_t disp_read, disp_write;

static void server(void* data) {

}

static void read_handler(void* data) {

}

static void write_handler(void* data) {

}

void dispatch_socket_read_write()
{
  MU_BEGIN_TEST(dispatch_socket_read_write);

  dispatch_async_f(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), NULL, server);

  sock_queue = dispatch_queue_create("de.mlba-team.xdispatch.socket_read_write", NULL);
  MU_ASSERT_NOT_NULL(sock_queue);

  disp_read = dispatch_source_create(DISPATCH_SOURCE_TYPE_READ, sock_fd, 0, sock_queue);
  MU_ASSERT_NOT_NULL(disp_read);

  disp_write = dispatch_source_create(DISPATCH_SOURCE_TYPE_WRITE, sock_fd, 0, sock_queue);
  MU_ASSERT_NOT_NULL(disp_write);

  MU_PASS("");
  dispatch_main();

  MU_FAIL("Should never reach this");
  MU_END_TEST;
}



#endif /* DISPATCH_SOURCE_HAS_READ && DISPATCH_SOURCE_HAS_READ */
