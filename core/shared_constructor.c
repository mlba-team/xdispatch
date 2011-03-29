
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

static inline int
_dispatch_rootq2wq_pri(long idx)
{
    switch (idx) {
    case 0:
        return WORKQ_LOW_PRIOQUEUE;
    case 1:
    default:
        return WORKQ_DEFAULT_PRIOQUEUE;
    case 2:
        return WORKQ_HIGH_PRIOQUEUE;
    }
}

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
        pthread_workqueue_attr_setqueuepriority_np(&attributes, _dispatch_rootq2wq_pri(i));
        pthread_workqueue_attr_setovercommit_np(&attributes, i & 1);
        pthread_workqueue_create_np(&(cast_queue(_dispatch_global_q[i])->pool), &attributes);
        assert(cast_queue(_dispatch_global_q[i])->pool);
        _dispatch_global_q[i]->type = DISPATCH_QUEUE;
    }
    pthread_workqueue_attr_destroy_np(&attributes);

    _dispatch_main_q = dispatch_queue_create("com.apple.main-thread",NULL);
    assert(cast_queue(_dispatch_main_q));
    _dispatch_main_q->type = DISPATCH_MAIN_QUEUE;
    cast_queue(_dispatch_main_q)->loop = _evt_create(_main_worker);
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
