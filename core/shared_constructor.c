
#include "queue_internal.h"

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
            cleanup
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif

void init(){
#ifdef PTHREAD_WORKQUEUE_USER_IMPLEMENTATION
    int pthread_workqueue_init_np(void);
#endif

}

void cleanup(){

}
