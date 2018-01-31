/*
* shared_constructor.cpp
*
* Copyright (c) 2011-2018 MLBA-Team
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

#include "xdispatch_internal.h"

__XDISPATCH_USE_NAMESPACE

#ifndef _WIN32

extern "C" int
__attribute(
    ( constructor )
) init( void );

extern "C" void
__attribute(
    ( destructor )
) cleanup( void );

#else // ifndef _WIN32

extern "C" int
init();

extern "C" void
cleanup();

// DllMain - see
// http://msdn.microsoft.com/en-us/library/ms682596%28v=vs.85%29.aspx
BOOL WINAPI DllMain(
    HINSTANCE hinstDLL, // handle to DLL module
    DWORD fdwReason, // reason for calling function
    LPVOID lpReserved
) // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason )
    {
    case DLL_PROCESS_ATTACH:

        // Initialize once for each new process.
        // Return FALSE to fail DLL load.
        if( init() < 0 )
        {
            return FALSE;
        }

        break;

    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        cleanup();
        break;

    case DLL_THREAD_DETACH:
        // Perform cleanup on a per thread base
        break;
    } // switch

    return TRUE; // Successful DLL_PROCESS_ATTACH.
} // DllMain


#endif // ifndef _WIN32

extern "C" int init()
{
    return 0;
}


extern "C" void cleanup() { }
