/*
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

#include "internal.h"

#ifndef _WIN32

int __attribute((constructor)) init(void);
void __attribute((destructor)) cleanup(void);

#else

int init();
void cleanup();

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
        if(init() < 0)
            return FALSE;
        break;

    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        cleanup();
        break;

    case DLL_THREAD_DETACH:
        // Perform cleanup on a per thread base
    {
		/*
        // workaround for TLS destructors on windows
        void* val = _dispatch_thread_getspecific(dispatch_queue_key);
        if(val) _dispatch_queue_cleanup(val);
        val = _dispatch_thread_getspecific(dispatch_sema4_key);
        if(val) dispatch_release(val);
        val = _dispatch_thread_getspecific(dispatch_cache_key);
        if(val) _dispatch_cache_cleanup2(val);
		*/
    }
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

#endif

int init(){
#if STATIC_WORKQUEUE
    if (pthread_workqueue_init_np() < 0)
        return (-1);
#endif
#if STATIC_KQUEUE
    if (libkqueue_init() < 0)
        return (-1);
#endif

    libdispatch_init();

    return 0;
}

void cleanup(){

}
