/*
* Copyright (c) 2008-2009 Apple Inc. All rights reserved.
* Copyright (c) 2011-2012 MLBA-Team. All rights reserved.
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


#include "../core/platform/atomic.h"
#include "tests.h"


/*
Test ensuring the atomic operations work the same on all platforms/compilers
 */

/*
  writes value into *ptr, and returns the previous
  contents of *ptr.
  */
static void test_atomic_xchg(){
    uintptr_t variable = 1;
    uintptr_t old = dispatch_atomic_xchg(&variable, 3);

    MU_DESC_ASSERT_EQUAL("xchg", old, 1);
    MU_DESC_ASSERT_EQUAL("xchg", variable, 3);
}

/*
  These builtins perform an atomic compare and swap.
  That is, if the current value of *ptr is oldval,
  then write newval into *ptr and return true (=1),
  false (=0) otherwise
  */
static void test_atomic_cmpxchg(){
    uintptr_t variable = 1;
    char changed = dispatch_atomic_cmpxchg(&variable, 4, 3);
    MU_DESC_ASSERT_NOT_TRUE("cmpxchg", changed);
    MU_DESC_ASSERT_EQUAL("cmpxchg", variable, 1);

    changed = dispatch_atomic_cmpxchg(&variable, 1, 3);
    MU_DESC_ASSERT_EQUAL("cmpxchg", variable, 3);
    MU_DESC_ASSERT_TRUE("cmpxchg", changed);
}

/*
  writes value into *ptr, and returns the previous
  contents of *ptr. (Same as atomic_xchg, but for pointers)
  */
static void test_atomic_ptr_xchg(){
    void* variable = (void*)0x01;
    void* oldval = dispatch_atomic_ptr_xchg(&variable, (void*)0x03);

    MU_DESC_ASSERT_EQUAL_HEX("ptr_xchg", oldval, (void*)0x01);
    MU_DESC_ASSERT_EQUAL_HEX("ptr_xchg", variable, (void*)0x03);
}


/*
  These builtins perform an atomic compare and swap.
  That is, if the current value of *ptr is oldval,
  then write newval into *ptr and return true (=1),
  false (=0) otherwise. (Same as atomic_cmpxchg, but for pointers)
  */
static void test_atomic_ptr_cmpxchg(){
    void* variable = (void*)0x01;
    char changed = dispatch_atomic_ptr_cmpxchg(&variable, (void*)0x04, (void*)0x03);
    MU_DESC_ASSERT_NOT_TRUE("ptr_cmpxchg", changed);
    MU_DESC_ASSERT_EQUAL("ptr_cmpxchg", variable, (void*)0x01);

    changed = dispatch_atomic_ptr_cmpxchg(&variable, (void*)0x01, (void*)0x03);
    MU_DESC_ASSERT_EQUAL("ptr_cmpxchg", variable, (void*)0x03);
    MU_DESC_ASSERT_TRUE("ptr_cmpxchg", changed);
}

/*
  Increase the value by one and return the NEW value
  */
static void test_atomic_inc(){
    uintptr_t variable = 1;
    uintptr_t newval = dispatch_atomic_inc(&variable);

    MU_DESC_ASSERT_EQUAL("inc", variable, 2);
    MU_DESC_ASSERT_EQUAL("inc", variable, newval);
}

/*
  Decrease the value by one and return the NEW value
  */
static void test_atomic_dec(){
    uintptr_t variable = 2;
    uintptr_t newval = dispatch_atomic_dec(&variable);

    MU_DESC_ASSERT_EQUAL("dec", variable, 1);
    MU_DESC_ASSERT_EQUAL("dec", variable, newval);
}

/*
  Add the given number to the value and return the NEW value
  */
static void test_atomic_add(){
    uintptr_t variable = 2;
    uintptr_t newval = dispatch_atomic_add(&variable, 2);

    MU_DESC_ASSERT_EQUAL("add", variable, 4);
    MU_DESC_ASSERT_EQUAL("add", variable, newval);
}

/*
  Subtract the given number from the value and return the NEW value
  */
static void test_atomic_sub(){
    uintptr_t variable = 2;
    uintptr_t newval = dispatch_atomic_sub(&variable, 2);

    MU_DESC_ASSERT_EQUAL("sub", variable, 0);
    MU_DESC_ASSERT_EQUAL("sub", variable, newval);
}

/*
  Store the ored combination and return the OLD value (before changing)
  */
static void test_atomic_or(){
    uintptr_t variable = 0;
    uintptr_t oldval = dispatch_atomic_or(&variable, 1);

    MU_DESC_ASSERT_EQUAL("or", variable, 1);
    MU_DESC_ASSERT_EQUAL("or", oldval, 0);
}

/*
  Store the anded combination and return the OLD value
  */
static void test_atomic_and(){
    uintptr_t variable = 1;
    uintptr_t oldval = dispatch_atomic_and(&variable, 0);

    MU_DESC_ASSERT_EQUAL("or", variable, 0);
    MU_DESC_ASSERT_EQUAL("or", oldval, 1);
}

/*
  Make sure all variables have the same value
  on all processors
  */
static void test_atomic_barrier(){
    // ?? How could we possibly verify this?
    dispatch_atomic_barrier();
}

void atomic_operations(){
    MU_BEGIN_TEST(atomic_operations);

    test_atomic_xchg();
    test_atomic_cmpxchg();
    test_atomic_ptr_xchg();
    test_atomic_ptr_cmpxchg();
    test_atomic_inc();
    test_atomic_dec();
    test_atomic_add();
    test_atomic_sub();
    test_atomic_or();
    test_atomic_and();
    test_atomic_barrier();
    MU_PASS("");

    MU_END_TEST;
}
