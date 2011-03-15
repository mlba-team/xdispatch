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

#include "MUnit_tools.h"
#include "private.h"
#include "typedefs.h"

item_t* suite;

void _begin_test(const char *name){
	printf("Running test '%s'...",name);
	fflush(stdout);
}

void _register_suite(mu_test_func function, const char *name){
	mu_test_t* t = malloc(sizeof(mu_test_t));
	t->function=function;
	t->name=name;
	append(suite,(void*)t);
}
