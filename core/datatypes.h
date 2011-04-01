/*
* Copyright (c) 2010 Apple Inc. All rights reserved.
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



#ifndef DATATYPES_H_
#define DATATYPES_H_

#include "taskqueue.h"
#include "internal.h"

#ifndef TRUE
#	define TRUE 1
#endif

#ifndef FALSE
#	define FALSE 0
#endif

enum _obj_types {
    DISPATCH_QUEUE,
    DISPATCH_SERIAL_QUEUE,
    DISPATCH_MAIN_QUEUE,
    DISPATCH_SEMAPHORE,
    DISPATCH_GROUP
};

// these two can easily be used to secure access to a dispatch_object
#define OBJ_SAFE_ENTER(Q) while(atomic_swap_get(&(Q->lock),1)==1) /*{ _d_suspend(1); } */
#define OBJ_SAFE_LEAVE(Q) atomic_swap_get(&(Q->lock),0)

// objects and accessors

#define cast_queue(A) ((_taskqueue_t)(A->obj))

typedef struct _group_obj_s {
    ATOMIC_INT count;
    dispatch_function_t notify;
    dispatch_queue_t queue;
    void* data;
}* _group_obj_t;

#define cast_group(A) ((_group_obj_t)(A->obj))

typedef struct _sem_obj_s {
    sem_t lock;
}* _sem_obj_t;

#define cast_sem(A) ((_sem_obj_t)(A->obj))

//
//	Returns an empty generic dispatch_object_t
//
static dispatch_object_t _get_empty_object(){
//    size_t size = 0;
//    size_t obj_size = 0;
//    if(sizeof(struct _sem_obj_s) > size)
//        size = sizeof(struct _sem_obj_s);
//    if(sizeof(struct _group_obj_s) > size)
//        size = sizeof(struct _group_obj_s);
//    if(sizeof(struct _taskqueue_s) > size)
//        size = sizeof(struct _taskqueue_s);

//    obj_size = sizeof(struct dispatch_object_s);
//    size = size + obj_size;

//    obj = (dispatch_object_t)malloc(size);

//    if(obj){
//        memset(obj,0,size);
//        obj->references = 1;
//        obj->type = -1;
//        obj->suspend_ct = 0;
//        obj->obj = ((char*)obj) + obj_size;
//    }

//    return obj;
    return NULL;
}

//
//	Completely destroys the given dispatch_object_t
//
static void _destroy_object(dispatch_object_t t){

//    assert(t);
//    //dispatch_debug(t,"Destroying");

//    switch(t->type){
//    case DISPATCH_QUEUE:
//    case DISPATCH_SERIAL_QUEUE:
//    case DISPATCH_MAIN_QUEUE:
//        _tq_clear(cast_queue(t));
//        break;
//    case DISPATCH_GROUP:
//        break;
//    case DISPATCH_SEMAPHORE:
//        sem_destroy(&(cast_sem(t)->lock));
//        break;
//    default:
//        break;
//    };

//    free(t);
}

#endif /* DATATYPES_H_ */
