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



#ifndef CONFIG_H_
#define CONFIG_H_

//
// various configuration options
//

// The number of threads
#define MAX_THREADS		_ideal_thread_count()*2

// The bonus times used for balancing the different priorities
#define BONUS_LOW		600/ _ideal_thread_count()
#define BONUS_DEFAULT	500/ _ideal_thread_count()
#define BONUS_HIGH		700/ _ideal_thread_count()

#endif /* CONFIG_H_ */
