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

#ifndef MUNIT_CXX_H_
#define MUNIT_CXX_H_

#define MU_ASSERT_THROW(X) \
     do { \
        try { X; } catch (...) { break; } \
        MU_FAIL("No Exception"); \
    } while (0);

#define MU_ASSERT_NO_THROW(X) \
    do { \
       try { X; } catch (...) { MU_FAIL("Catched exception"); } \
   } while (0);

#endif /* MUNIT_CXX_H_ */
