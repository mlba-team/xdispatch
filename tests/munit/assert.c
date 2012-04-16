/*
* Copyright (c) 2010 MLBA. All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// memory leak detection on windows
#if defined(_WIN32) && defined(DEBUG) && !defined(__GNUC__)
#	define _CRTDBG_MAP_ALLOC
#	include <stdlib.h>
#	include <crtdbg.h>
#	define	WIN32_LEAN_AND_MEAN 1
#	include <Windows.h>
#endif

#include "private.h"

void _pass(const char *msg, const char *file, int line){
    printf("    PASS\n");
    //#if defined(WIN32) && defined(DEBUG)
    //	Sleep(2000);
    //	_CrtDumpMemoryLeaks();
    //#endif
	exit(EXIT_SUCCESS);
}

void _pass_after_delay(void* d){
    printf("    PASS\n");
    //#if defined(WIN32) && defined(DEBUG)
    //	Sleep(2000);
    //	_CrtDumpMemoryLeaks();
    //#endif
	exit(EXIT_SUCCESS);
}

void _fail(const char *msg, const char *file, int line){
    printf(" FAIL:\n\t%s\n\t- at %s:%u\n",msg,file,line);
    //#if defined(WIN32) && defined(DEBUG)
    //	Sleep(2000);
    //	_CrtDumpMemoryLeaks();
    //#endif
	exit(EXIT_FAILURE);
}

#ifdef _MSC_VER
# define inline _inline
#endif

static inline void _assert_eval(const char* msg, char cond, const char* file, int line){
    if(!cond) {
        _fail(msg,file,line);
    } else if(verbose) {
        printf("\t%s\n\tOK\n", msg);
        fflush(stdout);
    }
}

void _assert_true_long(long a, long b, char cond, const char* cond_desc, const char* file, int line){
    char msg[512];
#ifndef _MSC_VER
    sprintf(msg,"Assert %s \n\t- is %li, %li",cond_desc, a, b);
#else
    sprintf_s(msg, 500,"Assert %s \n\t- is %li, %li",cond_desc, a, b);
#endif
    _assert_eval(msg, cond, file, line);
}

void _assert_true_hex(long a, long b, char cond, const char* cond_desc, const char* file, int line){
    char msg[512];
#ifndef _MSC_VER
    sprintf(msg,"Assert %s \n\t- is 0x%lo, 0x%lo",cond_desc, a, b);
#else
    sprintf_s(msg, 500,"Assert %s \n\t- is 0x%lo, 0x%lo",cond_desc, a, b);
#endif
    _assert_eval(msg, cond, file, line);
}

void _assert_true_double(double a, double b, char cond, const char* cond_desc, const char* file, int line){
    char msg[512];
#ifndef _MSC_VER
    sprintf(msg,"Assert %s \n\t- is %f, %f", cond_desc, a, b);
#else
    sprintf_s(msg, 500,"Assert %s \n\t- is %f, %f", cond_desc, a, b);
#endif
    _assert_eval(msg, cond, file, line);
}
