/*
* MUnit_assert.h
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

#ifndef MUNIT_ASSERT_H_
#define MUNIT_ASSERT_H_

/**
  Assertions for testing
  */

#define MU_PASS( MSG ) _pass( MSG, __FILE__, __LINE__ )
void
_pass(
    const char *msg,
    const char *file,
    int line
);

#define MU_PASS_AFTER_DELAY _pass_after_delay
void
_pass_after_delay(
    void *
);

#define MU_FAIL( MSG ) _fail( MSG, __FILE__, __LINE__ )
void
_fail(
    const char *msg,
    const char *file,
    int line
);

/* Assertions including a description */

#define MU_EX_L( C ) long mu_rC = (long)( C )
#define MU_EX_D( C ) double mu_rC = (double)( C )
#define MU_EX_VAL mu_rC
#define MU_EX_L1( C ) long mu_rC1 = (long)( C )
#define MU_EX_D1( C ) double mu_rC1 = (double)( C )
#define MU_EX_STR1( C ) std::string mu_rC1 = ( C )
#define MU_EX_VAL1 mu_rC1
#define MU_EX_L2( C ) long mu_rC2 = (long)( C )
#define MU_EX_D2( C ) double mu_rC2 = (double)( C )
#define MU_EX_STR2( C ) std::string mu_rC2 = ( C )
#define MU_EX_VAL2 mu_rC2

#define MU_DESC_ASSERT_TRUE( D, C ) { MU_EX_L( C );_assert_true_long( MU_EX_VAL, 0, (char)MU_EX_VAL, D ": "#C, __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_TRUE_HEX( D, C ) { MU_EX_L( C );_assert_true_hex( MU_EX_VAL, 0, (char)MU_EX_VAL, D ": "#C, __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_TRUE_DOUBLE( D, C ) { MU_EX_D( C );_assert_true_double( MU_EX_VAL, 0, (char)MU_EX_VAL, D ": "#C, __FILE__, __LINE__ );}
void
_assert_true_long(
    long a,
    long b,
    char cond,
    const char *cond_desc,
    const char *file,
    int line
);

void
_assert_true_hex(
    long a,
    long b,
    char cond,
    const char *cond_desc,
    const char *file,
    int line
);

void
_assert_true_double(
    double a,
    double b,
    char cond,
    const char *cond_desc,
    const char *file,
    int line
);

#define MU_DESC_ASSERT_NOT_TRUE( D, C ) { MU_EX_L( C );_assert_true_long( MU_EX_VAL, 0, MU_EX_VAL == 0, D ": "#C " == 0", __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_NOT_TRUE_HEX( D, C ) { MU_EX_L( C );_assert_true_long_hex( MU_EX_VAL, 0, MU_EX_VAL == 0, D ": !"#C " == 0", __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_NOT_TRUE_DOUBLE( D, C ) { MU_EX_D( C );_assert_true_double( MU_EX_VAL, 0, MU_EX_VAL == 0.0, D ": "#C " == 0.0", __FILE__, __LINE__ );}

#define MU_DESC_ASSERT_NULL( D, A ) { MU_EX_L( A );_assert_true_long( MU_EX_VAL, 0, MU_EX_VAL == 0, D ": "#A " == 0", __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_NOT_NULL( D, A ) { MU_EX_L( A );_assert_true_long( MU_EX_VAL, 0, MU_EX_VAL != 0, D ": "#A " != 0", __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_NULL_HEX( D, A ) { MU_EX_L( A );_assert_true_hex( MU_EX_VAL, 0, MU_EX_VAL == 0, D ": "#A " == 0", __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_NOT_NULL_HEX( D, A ) { MU_EX_L( A );_assert_true_hex( MU_EX_VAL, 0, MU_EX_VAL != 0, D ": "#A " != 0", __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_NULL_DOUBLE( D, A ) { MU_EX_D( A );_assert_true_double( MU_EX_VAL, 0, MU_EX_VAL == 0, D ": "#A " == 0", __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_NOT_NULL_DOUBLE( D, A ) { MU_EX_D( A );_assert_true_double( MU_EX_VAL, 0, MU_EX_VAL != 0, D ": "#A " != 0", __FILE__, __LINE__ );}

#define MU_DESC_ASSERT_EQUAL( D, A, B ) { MU_EX_L1( A );MU_EX_L2( B );_assert_true_long( MU_EX_VAL1, MU_EX_VAL2, MU_EX_VAL1 == MU_EX_VAL2, D ": "#A " == "#B, __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_EQUAL_HEX( D, A, B ) { MU_EX_L1( A );MU_EX_L2( B );_assert_true_hex( MU_EX_VAL1, MU_EX_VAL2, MU_EX_VAL1 == MU_EX_VAL2, D ": "#A " == "#B, __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_EQUAL_DOUBLE( D, A, B ) { MU_EX_D1( A );MU_EX_D2( B );_assert_true_double( MU_EX_VAL1, MU_EX_VAL2, MU_EX_VAL1 == MU_EX_VAL2, D ": "#A " == "#B, __FILE__, __LINE__ );}

#define MU_DESC_ASSERT_NOT_EQUAL( D, A, B ) { MU_EX_L1( A );MU_EX_L2( B );_assert_true_long( MU_EX_VAL1, MU_EX_VAL2, MU_EX_VAL1 != MU_EX_VAL2, D ": "#A " != "#B, __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_NOT_EQUAL_HEX( D, A, B ) { MU_EX_L1( A );MU_EX_L2( B );_assert_true_long_hex( MU_EX_VAL1, MU_EX_VAL2, MU_EX_VAL1 != MU_EX_VAL2, D ": "#A " != "#B, __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_NOT_EQUAL_DOULBE( D, A, B ) { MU_EX_D1( A );MU_EX_D2( B );_assert_true_double( MU_EX_VAL1, MU_EX_VAL2, MU_EX_VAL1 != MU_EX_VAL2, D ": "#A " != "#B, __FILE__, __LINE__ );}

#define MU_DESC_ASSERT_LESS_THAN( D, A, B ) { MU_EX_L1( A );MU_EX_L2( B );_assert_true_long( MU_EX_VAL1, MU_EX_VAL2, MU_EX_VAL1 < MU_EX_VAL2, D ": "#A " < "#B, __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_GREATER_THAN( D, A, B ) { MU_EX_L1( A );MU_EX_L2( B );_assert_true_long( MU_EX_VAL1, MU_EX_VAL2, MU_EX_VAL1 > MU_EX_VAL2, D ": "#A " > "#B, __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_LESS_THAN_DOUBLE( D, A, B ) { MU_EX_D1( A );MU_EX_D2( B );_assert_true_double( MU_EX_VAL1, MU_EX_VAL2, MU_EX_VAL1 < MU_EX_VAL2, D ": "#A " < "#B, __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_GREATER_THAN_DOUBLE( D, A, B ) { MU_EX_D1( A );MU_EX_D2( B );_assert_true_double( MU_EX_VAL1, MU_EX_VAL2, MU_EX_VAL1 > MU_EX_VAL2, D ": "#A " > "#B, __FILE__, __LINE__ );}

#define MU_DESC_ASSERT_LESS_THAN_EQUAL( D, A, B ) { MU_EX_L1( A );MU_EX_L2( B );_assert_true_long( MU_EX_VAL1, MU_EX_VAL2, MU_EX_VAL1 <= MU_EX_VAL2, D ": "#A " <= "#B, __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_GREATER_THAN_EQUAL( D, A, B ) { MU_EX_L1( A );MU_EX_L2( B );_assert_true_long( MU_EX_VAL1, MU_EX_VAL2, MU_EX_VAL1 >= MU_EX_VAL2, D ": "#A " >= "#B, __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_LESS_THAN_EQUAL_DOUBLE( D, A, B ) { MU_EX_D1( A );MU_EX_D2( B );_assert_true_double( MU_EX_VAL1, MU_EX_VAL2, MU_EX_VAL1 <= MU_EX_VAL2, D ": "#A " <= "#B, __FILE__, __LINE__ );}
#define MU_DESC_ASSERT_GREATER_THAN_EQUAL_DOUBLE( D, A, B ) { MU_EX_D1( A );MU_EX_D2( B );_assert_true_double( MU_EX_VAL1, MU_EX_VAL2, MU_EX_VAL1 >= MU_EX_VAL2, D ": "#A " >= "#B, __FILE__, __LINE__ );}

/* Assertions with no description */

#define MU_ASSERT_TRUE( C ) MU_DESC_ASSERT_TRUE( "", C )
#define MU_ASSERT_TRUE_HEX( C ) MU_DESC_ASSERT_TRUE_HEX( "", C )
#define MU_ASSERT_TRUE_DOUBLE( C ) MU_DESC_ASSERT_NOT_TRUE_DOUBLE( "", C )

#define MU_ASSERT_NOT_TRUE( C ) MU_DESC_ASSERT_NOT_TRUE( "", C )
#define MU_ASSERT_NOT_TRUE_HEX( C ) MU_DESC_ASSERT_NOT_TRUE_HEX( "", C )
#define MU_ASSERT_NOT_TRUE_DOUBLE( C ) MU_DESC_ASSERT_NOT_TRUE_DOUBLE( "", C )

#define MU_ASSERT_NULL( A ) MU_DESC_ASSERT_NULL( "", A )
#define MU_ASSERT_NOT_NULL( A ) MU_DESC_ASSERT_NOT_NULL( "", A )
#define MU_ASSERT_NULL_HEX( A ) MU_DESC_ASSERT_NULL_HEX( "", A )
#define MU_ASSERT_NOT_NULL_HEX( A ) MU_DESC_ASSERT_NOT_NULL_HEX( "", A )
#define MU_ASSERT_NULL_DOUBLE( A ) MU_DESC_ASSERT_NULL_DOUBLE( "", A )
#define MU_ASSERT_NOT_NULL_DOUBLE( A ) MU_DESC_ASSERT_NOT_NULL_DOUBLE( "", A )

#define MU_ASSERT_EQUAL( A, B ) MU_DESC_ASSERT_EQUAL( "", A, B )
#define MU_ASSERT_EQUAL_HEX( A, B ) MU_DESC_ASSERT_EQUAL_HEX( "", A, B )
#define MU_ASSERT_EQUAL_DOUBLE( A, B ) MU_DESC_ASSERT_EQUAL_DOUBLE( "", A, B )
#define MU_ASSERT_NOT_EQUAL( A, B ) MU_DESC_ASSERT_NOT_EQUAL( "", A, B )
#define MU_ASSERT_NOT_EQUAL_HEX( A, B ) MU_DESC_ASSERT_NOT_EQUAL_HEX( "", A, B )
#define MU_ASSERT_NOT_EQUAL_DOULBE( A, B ) MU_DESC_ASSERT_NOT_EQUAL_DOULBE( "", A, B )

#define MU_ASSERT_LESS_THAN( A, B ) MU_DESC_ASSERT_LESS_THAN( "", A, B )
#define MU_ASSERT_GREATER_THAN( A, B ) MU_DESC_ASSERT_GREATER_THAN( "", A, B )
#define MU_ASSERT_LESS_THAN_DOUBLE( A, B ) MU_DESC_ASSERT_LESS_THAN_DOUBLE( "", A, B )
#define MU_ASSERT_GREATER_THAN_DOUBLE( A, B ) MU_DESC_ASSERT_GREATER_THAN_DOUBLE( "", A, B )
#define MU_ASSERT_LESS_THAN_EQUAL( A, B ) MU_DESC_ASSERT_LESS_THAN_EQUAL_EQUAL( "", A, B )
#define MU_ASSERT_GREATER_THAN_EQUAL( A, B ) MU_DESC_ASSERT_GREATER_THAN_EQUAL( "", A, B )
#define MU_ASSERT_LESS_THAN_EQUAL_DOUBLE( A, B ) MU_DESC_ASSERT_LESS_THAN_EQUAL_DOUBLE( "", A, B )
#define MU_ASSERT_GREATER_THAN_EQUAL_DOUBLE( A, B ) MU_DESC_ASSERT_GREATER_THAN_EQUAL_DOUBLE( "", A, B )

#endif /* MUNIT_ASSERT_H_ */