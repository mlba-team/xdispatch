/*
* MUnit.h
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

#ifndef MUNIT_H_
#define MUNIT_H_

/**
  The core header. Include this header in any file
  you want to include your tests in.
  */

#ifdef __cplusplus
extern "C" {
#endif

#include "MUnit_assert.h"
#include "MUnit_runner.h"
#include "MUnit_tools.h"

#ifdef __cplusplus
}

 # include "MUnit_cxx.h"
#endif // ifdef __cplusplus

#endif /* MUNIT_H_ */