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



#ifndef EXECUTION_H_
#define EXECUTION_H_

#include "queue_internal.h"

//
// @file execution.h
// Executes any jobs found on one of the three global queues
// 

//
// Signals the main queue that work has to be done
// @param q A pointer to the main queue
//
void _notify_main(dispatch_queue_t q);

//
// Assigns a new workitem to the given global queue
// in order to make the threadpool execute it
// @param q A pointer to the global queue
//
void _notify_global(dispatch_queue_t q);

//
// Handles all items on the queues
// @param q The queue to handle
// 
void _loop_worker(void*);

//
// Handles all current items on the main queue
//
void _main_worker(_evt_loop_t loop, int revent, void* data);

//
// Traces the events on the give queue
// and handles them
// 
void _timer_worker(_evt_loop_t loop, int revent, void* data);

//
// Handles one item on the serial queue and dispatches
// the next for handling
// 
void _serial_worker(void* context);

#endif /* EXECUTION_H_ */
