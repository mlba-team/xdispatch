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
#if defined(WIN32) && defined(DEBUG)
#	define _CRTDBG_MAP_ALLOC
#	include <stdlib.h>
#	include <crtdbg.h>
#	include <Windows.h>
#endif

#include "MUnit_assert.h"

void _pass(const char *msg, const char *file, int line){
	printf(" Passed\n");
//#if defined(WIN32) && defined(DEBUG)
//	Sleep(2000);
//	_CrtDumpMemoryLeaks();
//#endif
	exit(EXIT_SUCCESS);
}

void _pass_after_delay(void* d){
	printf(" Passed\n");
//#if defined(WIN32) && defined(DEBUG)
//	Sleep(2000);
//	_CrtDumpMemoryLeaks();
//#endif
	exit(EXIT_SUCCESS);
}

void _fail(const char *msg, const char *file, int line){
	printf("Failed\n\t%s, at: %s:%u\n",msg,file,line);
//#if defined(WIN32) && defined(DEBUG)
//	Sleep(2000);
//	_CrtDumpMemoryLeaks();
//#endif
	exit(EXIT_FAILURE);
}

void _assert_true(bool cond, const char *cond_desc, const char *file, int line){
	char msg[100];
	if(!cond) {
#ifndef WIN32_VS
		sprintf(msg,"Test %s failed",cond_desc);
#else
		sprintf_s(msg, 100,"Test %s failed",cond_desc);
#endif
		_fail(msg,file,line);
	}
}

void _assert_not_true(bool cond, const char *cond_desc, const char *file, int line){
	char msg[100];
	if(cond) {
#ifndef WIN32_VS
		sprintf(msg,"Test not %s failed",cond_desc);
#else
		sprintf_s(msg, 100,"Test not %s failed",cond_desc);
#endif
		_fail(msg,file,line);
	}
}
