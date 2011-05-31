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

/* Assertions including a description */

#define MU_DESC_ASSERT_TRUE(D,C) _assert_true_long((long)(C), 0, (C), D": "#C,__FILE__,__LINE__)
#define MU_DESC_ASSERT_TRUE_HEX(D,C) _assert_true_hex((long)(C), 0, (C), D": "#C,__FILE__,__LINE__)
#define MU_DESC_ASSERT_TRUE_DOUBLE(D,C) _assert_true_double((C), 0, (C), D": "#C,__FILE__,__LINE__)
void _assert_true_long(long a, long b, bool cond, const char* cond_desc, const char* file, int line);
void _assert_true_hex(long a, long b, bool cond, const char* cond_desc, const char* file, int line);
void _assert_true_double(double a, double b, bool cond, const char* cond_desc, const char* file, int line);

#define MU_DESC_ASSERT_NOT_TRUE(D,C) _assert_not_true_long((long)(C), 0, (C)==0, D": "#C" == 0",__FILE__,__LINE__)
#define MU_DESC_ASSERT_NOT_TRUE_HEX(D,C) _assert_not_true_long_hex((long)(C), 0, (C)==0, D": !"#C" == 0",__FILE__,__LINE__)
#define MU_DESC_ASSERT_NOT_TRUE_DOUBLE(D,C) _assert_not_true_double((C), 0, (C)==0.0, D": "#C" == 0.0",__FILE__,__LINE__)
void _assert_not_true_long(long a, long b, bool cond,  const char* cond_desc, const char* file, int line);
void _assert_not_true_hex(long a, long b, bool cond,  const char* cond_desc, const char* file, int line);
void _assert_not_true_double(double a, double b, bool cond,  const char* cond_desc, const char* file, int line);

#define MU_DESC_ASSERT_NULL(D,A) _assert_true_long((long)(A), 0, (A)==0,D": "#A" == 0",__FILE__,__LINE__)
#define MU_DESC_ASSERT_NOT_NULL(D,A) _assert_true_long((long)(A), 0, (A)!=0,D": "#A" != 0",__FILE__,__LINE__)
#define MU_DESC_ASSERT_NULL_HEX(D,A) _assert_true_hex((long)(A), 0, (A)==0,D": "#A" == 0",__FILE__,__LINE__)
#define MU_DESC_ASSERT_NOT_NULL_HEX(D,A) _assert_true_hex((long)(A), 0, (A)!=0,D": "#A" != 0",__FILE__,__LINE__)
#define MU_DESC_ASSERT_NULL_DOUBLE(D,A) _assert_true_double((A), 0, (A)==0,D": "#A" == 0",__FILE__,__LINE__)
#define MU_DESC_ASSERT_NOT_NULL_DOUBLE(D,A) _assert_true_double((A), 0, (A)!=0,D": "#A" != 0",__FILE__,__LINE__)

#define MU_DESC_ASSERT_EQUAL(D,A,B) _assert_true_long((long)(A),(long)(B),(A)==(B),D": "#A" == "#B,__FILE__,__LINE__)
#define MU_DESC_ASSERT_EQUAL_HEX(D,A,B) _assert_true_hex((long)(A),(long)(B),(A)==(B),D": "#A" == "#B,__FILE__,__LINE__)
#define MU_DESC_ASSERT_EQUAL_DOUBLE(D,A,B) _assert_true_double((A),(B),(A)==(B),D": "#A" == "#B,__FILE__,__LINE__)

#define MU_DESC_ASSERT_NOT_EQUAL(D,A,B) _assert_true_long((long)(A),(long)(B),(A)!=(B),D": "#A" != "#B,__FILE__,__LINE__)
#define MU_DESC_ASSERT_NOT_EQUAL_HEX(D,A,B) _assert_true_long_hex((long)(A),(long)(B),(A)!=(B),D": "#A" != "#B,__FILE__,__LINE__)
#define MU_DESC_ASSERT_NOT_EQUAL_DOULBE(D,A,B) _assert_true_double((A),(B),(A)!=(B),D": "#A" != "#B,__FILE__,__LINE__)

#define MU_DESC_ASSERT_LESS_THAN(D,A,B) _assert_true_long((long)(A),(long)(B),(A)<(B),D": "#A" < "#B,__FILE__,__LINE__)
#define MU_DESC_ASSERT_GREATER_THAN(D,A,B) _assert_true_long((long)(A),(long)(B),(A)>(B),D": "#A" > "#B,__FILE__,__LINE__)
#define MU_DESC_ASSERT_LESS_THAN_DOUBLE(D,A,B) _assert_true_double((A),(B),(A)<(B),D": "#A" < "#B,__FILE__,__LINE__)
#define MU_DESC_ASSERT_GREATER_THAN_DOUBLE(D,A,B) _assert_true_double((A),(B),(A)>(B),D": "#A" > "#B,__FILE__,__LINE__)

/* Assertions with no description */

#define MU_ASSERT_TRUE(C) _assert_true_long((long)(C), 0, (C), #C,__FILE__,__LINE__)
#define MU_ASSERT_TRUE_HEX(C) _assert_true_hex((long)(C), 0, (C), #C,__FILE__,__LINE__)
#define MU_ASSERT_TRUE_DOUBLE(C) _assert_true_double((C), 0, (C), #C,__FILE__,__LINE__)

#define MU_ASSERT_NOT_TRUE(C) _assert_not_true_long((long)(C), 0, (C), #C,__FILE__,__LINE__)
#define MU_ASSERT_NOT_TRUE_HEX(C) _assert_not_true_hex((long)(C), 0, (C), #C,__FILE__,__LINE__)
#define MU_ASSERT_NOT_TRUE_DOUBLE(C) _assert_not_true_double((C), 0, (C), #C,__FILE__,__LINE__)

#define MU_ASSERT_NULL(A) _assert_true_long((long)(A), 0, (A)==0,#A" == 0",__FILE__,__LINE__)
#define MU_ASSERT_NOT_NULL(A) _assert_true_long((long)(A), 0, (A)!=0,#A" != 0",__FILE__,__LINE__)
#define MU_ASSERT_NULL_HEX(A) _assert_true_hex((long)(A), 0, (A)==0,#A" == 0",__FILE__,__LINE__)
#define MU_ASSERT_NOT_NULL_HEX(A) _assert_true_hex((long)(A), 0, (A)!=0,#A" != 0",__FILE__,__LINE__)
#define MU_ASSERT_NULL_DOUBLE(A) _assert_true_double((A), 0, (A)==0,#A" == 0",__FILE__,__LINE__)
#define MU_ASSERT_NOT_NULL_DOUBLE(A) _assert_true_double((A), 0, (A)!=0,#A" != 0",__FILE__,__LINE__)

#define MU_ASSERT_EQUAL(A,B) _assert_true_long((long)(A),(long)(B),(A)==(B),#A" == "#B,__FILE__,__LINE__)
#define MU_ASSERT_EQUAL_HEX(A,B) _assert_true_hex((long)(A),(long)(B),(A)==(B),#A" == "#B,__FILE__,__LINE__)
#define MU_ASSERT_EQUAL_DOUBLE(A,B) _assert_true_double((A),(B),(A)==(B),#A" == "#B,__FILE__,__LINE__)

#define MU_ASSERT_NOT_EQUAL(A,B) _assert_true_long((long)(A),(long)(B),(A)!=(B),#A" != "#B,__FILE__,__LINE__)
#define MU_ASSERT_NOT_EQUAL_HEX(A,B) _assert_true_hex((long)(A),(long)(B),(A)!=(B),#A" != "#B,__FILE__,__LINE__)
#define MU_ASSERT_NOT_EQUAL_DOULBE(A,B) _assert_true_double((A),(B),(A)!=(B),#A" != "#B,__FILE__,__LINE__)

#define MU_ASSERT_LESS_THAN(A,B) _assert_true_long((long)(A),(long)(B),(A)<(B),#A" < "#B,__FILE__,__LINE__)
#define MU_ASSERT_GREATER_THAN(A,B) _assert_true_long((long)(A),(long)(B),(A)>(B),#A" > "#B,__FILE__,__LINE__)
#define MU_ASSERT_LESS_THAN_DOUBLE(A,B) _assert_true_double((A),(B),(A)<(B),#A" < "#B,__FILE__,__LINE__)
#define MU_ASSERT_GREATER_THAN_DOUBLE(A,B) _assert_true_double((A),(B),(A)>(B),#A" > "#B,__FILE__,__LINE__)

#endif /* MUNIT_ASSERT_H_ */
