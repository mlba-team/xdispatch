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

#ifndef MUNIT_ASSERT_H_
#define MUNIT_ASSERT_H_

#include "typedefs.h"

/**
  Assertions for testing
  */

#define MU_PASS(MSG) _pass(MSG, __FILE__, __LINE__)
void _pass(const char* msg, const char* file, int line);

#define MU_PASS_AFTER_DELAY _pass_after_delay
void _pass_after_delay(void*);

#define MU_FAIL(MSG) _fail(MSG, __FILE__, __LINE__)
void _fail(const char* msg, const char* file, int line);

#define MU_ASSERT_TRUE(C) _assert_true(C,#C,__FILE__,__LINE__)
void _assert_true(bool cond, const char* cond_desc, const char* file, int line);

#define MU_ASSERT_NOT_TRUE(C) _assert_not_true(C,#C,__FILE__,__LINE__)
void _assert_not_true(bool cond,  const char* cond_desc, const char* file, int line);

#define MU_ASSERT_NULL(A) _assert_true(A==0,#A" == 0",__FILE__,__LINE__)
#define MU_ASSERT_NOT_NULL(A) _assert_true(A!=0,#A" != 0",__FILE__,__LINE__)

#define MU_ASSERT_EQUAL(A,B) _assert_true(A==B,#A" == "#B,__FILE__,__LINE__)
#define MU_ASSERT_EQUAL_LONG(A,B) _assert_true(A==B,#A" == "#B,__FILE__,__LINE__)

#define MU_ASSERT_NOT_EQUAL(A,B) _assert_true(A!=B,#A" != "#B,__FILE__,__LINE__)
#define MU_ASSERT_NOT_EQUAL_LONG(A,B) _assert_true(A!=B,#A" != "#B,__FILE__,__LINE__)

#define MU_ASSERT_EQUAL_DOUBLE(A,B) _assert_true(A==B,#A" == "#B,__FILE__,__LINE__)

#define MU_ASSERT_NOT_EQUAL_double(A,B) _assert_true(A!=B,#A" != "#B,__FILE__,__LINE__)

#endif /* MUNIT_ASSERT_H_ */
