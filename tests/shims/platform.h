/*
* Copyright (c) 2011 MLBA. All rights reserved.
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

#ifndef __TEST_SHIMS_PLATFORM_H_
#define __TEST_SHIMS_PLATFORM_H_

#ifdef _WIN32
# include "os_windows.h"
# include "threads_windows.h"
#else
# include "os_posix.h"
# include "threads_posix.h"
#endif

#endif /* __TEST_SHIMS_PLATFORM_H_ */
