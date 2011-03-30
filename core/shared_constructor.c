
#include "internal.h"

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
            _thread_man_init();
            break;

        case DLL_PROCESS_DETACH:
         // Perform any necessary cleanup.
            _thread_man_cleanup();
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif

void init(){
#ifdef PTHREAD_WORKQUEUE_USER_IMPLEMENTATION
    int pthread_workqueue_init_np(void);
#endif

    // init thread manager
    _thread_man_init();

    // init the root queues
    _dispatch_root_queues_init();
}

void cleanup(){
    //int i = 0;

    _thread_man_cleanup();

   /* for(i = 0; i < 3; i++) {
#ifdef DEBUG
        dispatch_debug(_dispatch_global_q[i], "release during cleanup");
#endif
        dispatch_release(_dispatch_global_q[i]);
    }
#ifdef DEBUG
        dispatch_debug(_dispatch_main_q, "release during cleanup");
#endif
    dispatch_release(_dispatch_main_q); */
}
