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

#include "internal.h"

void init();
void cleanup();

#ifndef _WIN32

    void __attribute((constructor)) init(void);
    void __attribute((destructor)) cleanup(void);

#else

// DllMain - see http://msdn.microsoft.com/en-us/library/ms682596%28v=vs.85%29.aspx
BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason )
    {
        case DLL_PROCESS_ATTACH:
         // Initialize once for each new process.
         // Return FALSE to fail DLL load.
            init();
            break;

        case DLL_PROCESS_DETACH:
         // Perform any necessary cleanup.
            cleanup();
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

void pthread_workqueue_init_np();

#endif

void init(){
#ifdef PTHREAD_WORKQUEUE_USER_IMPLEMENTATION
    pthread_workqueue_init_np();
#endif

    libdispatch_init();

//    // init thread manager
//    _thread_man_init();

//    // init the root queues
//    _dispatch_root_queues_init();
}

void cleanup(){
//    //int i = 0;

//    _thread_man_cleanup();

//   /* for(i = 0; i < 3; i++) {
//#ifdef DEBUG
//        dispatch_debug(_dispatch_global_q[i], "release during cleanup");
//#endif
//        dispatch_release(_dispatch_global_q[i]);
//    }
//#ifdef DEBUG
//        dispatch_debug(_dispatch_main_q, "release during cleanup");
//#endif
//    dispatch_release(_dispatch_main_q); */
}
