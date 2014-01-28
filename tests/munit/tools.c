/*
* tools.c
* 
* Copyright (c) 2011-2014 MLBA-Team
* All rights reserved.
* 
* @LICENSE_HEADER_START@
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
* @LICENSE_HEADER_END@
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "private.h"

item_t *suite;
MU_messageHandler msg_handler;

void _begin_test(
    const char *name
)
{
#ifndef _WIN32
    pid_t pid = getpid();
#else
    int pid = GetCurrentProcessId();
#endif

    if( verbose )
        printf( "[%i] Running test '%s'...\n", pid, name );
    else
        printf( "Running test '%s'...", name );

    fflush( stdout );
}


void _register_suite(
    mu_test_func function,
    const char *name
)
{
    mu_test_t *t = malloc( sizeof( mu_test_t ) );

    t->function = function;
    t->name = name;
    append( suite, (void *)t );
}