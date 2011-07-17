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

#ifndef MUNIT_TOOLS_H_
#define MUNIT_TOOLS_H_

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN 1
# include <windows.h>
# define MU_SLEEP(T) Sleep(1000*T)
#else
# include <unistd.h>
# define MU_SLEEP(T) sleep(T)
#endif

#include "list.h"

typedef void(*mu_test_func)();

extern item_t* suite;

#define MU_REGISTER_TEST(NAME) _register_suite(NAME,#NAME)
void _register_suite(mu_test_func function, const char* name);

#define MU_BEGIN_TEST(NAME) _begin_test(#NAME)
void _begin_test(const char* name);

#define MU_END_TEST

#define MU_MESSAGE(format,...) _print_message(format, ## __VA_ARGS__)
static void _print_message(const char* format, ...){
		static char was_called = 0;
		va_list params;
		char tmp[512];
		if(!was_called){
			was_called = 1;
			printf("\n");
		}
		va_start(params, format);
#ifdef WIN32_VS
		vsprintf_s(tmp, 510, format, params);
#else
		vsprintf(tmp,format,params);
#endif
		va_end(params);
		printf("\t-> %s\n",tmp);
		fflush(stdout);
}

#endif /* MUNIT_TOOLS_H_ */
