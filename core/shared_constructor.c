
#include "queue_internal.h"

#define LOW_POOL_PRIORITY 2
#define DEFAULT_POOL_PRIORITY 0
#define HIGH_POOL_PRIORITY 1

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
    int i = 0;
    pthread_workqueue_attr_t attributes;

#ifdef PTHREAD_WORKQUEUE_USER_IMPLEMENTATION
    int pthread_workqueue_init_np(void);
#endif

    // init thread manager
    _thread_man_init();

    // create global queues
    pthread_workqueue_attr_init_np(&attributes);
    for(i = 0; i < 3; i++) {
        _dispatch_global_q[i] = dispatch_queue_create(_dispatch_global_queues[i],NULL);
        assert(cast_queue(_dispatch_global_q[i]));
        switch(i) {
        case 2:
            pthread_workqueue_attr_setqueuepriority_np(&attributes, HIGH_POOL_PRIORITY);
            break;
        case 1:
            pthread_workqueue_attr_setqueuepriority_np(&attributes, DEFAULT_POOL_PRIORITY);
            break;
        case 0:
            pthread_workqueue_attr_setqueuepriority_np(&attributes, LOW_POOL_PRIORITY);
            break;
        }
        pthread_workqueue_create_np(&(cast_queue(_dispatch_global_q[i])->pool), &attributes);
        assert(cast_queue(_dispatch_global_q[i])->pool);
    }
    _dispatch_main_q = dispatch_queue_create("com.apple.main-thread",NULL);
    assert(cast_queue(_dispatch_main_q));
    _dispatch_main_q->type = DISPATCH_MAIN_QUEUE;
    cast_queue(_dispatch_main_q)->loop = _evt_create(_main_worker);
}

void cleanup(){
    int i = 0;

    _thread_man_cleanup();

    for(i = 0; i < 3; i++)
        dispatch_release(_dispatch_global_q[i]);
    dispatch_release(_dispatch_main_q);
}
