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


#include "queue_internal.h"

#define _dispatch_Block_copy(x) ((typeof(x))_dispatch_Block_copy(x))

dispatch_group_t
dispatch_group_create(void){
         dispatch_group_t g = (dispatch_group_t)_get_empty_object();
         if(!g)
             return NULL;

		 g->type = DISPATCH_GROUP;
         return g;
}

struct group_dt_s {
    void* context;
    dispatch_function_t work;
    dispatch_group_t obj;
};

void _group_helper(void* context){
    struct group_dt_s* dt = (struct group_dt_s*)context;
    _group_obj_t group = cast_group(dt->obj);

	assert(context);

    dt->work(dt->context);
    if(atomic_dec_get(&group->count)==0 && group->notify)
            dispatch_async_f(group->queue,group->data,group->notify);
   
    dispatch_release(dt->obj);
    free(dt);
}

void
dispatch_group_async_f(dispatch_group_t group, dispatch_queue_t queue, void *context, dispatch_function_t work){
        struct group_dt_s* dt = (struct group_dt_s*)malloc(sizeof(struct group_dt_s));
        
		assert(dt);

        dt->obj = group;
        dt->work = work;
        dt->context = context;
        atomic_inc_get(&cast_group(group)->count);
        dispatch_retain(group);
        dispatch_async_f(queue, dt, _group_helper);
}

#ifdef __BLOCKS__
void
dispatch_group_async(dispatch_group_t dg, dispatch_queue_t dq, dispatch_block_t db)
{
    dispatch_group_async_f(dg, dq, _dispatch_Block_copy(db), _dispatch_call_block_and_release);
}
#endif

long
dispatch_group_wait(dispatch_group_t group, dispatch_time_t timeout){
	dispatch_time_t now = 0;

    while(cast_group(group)->count>0 && timeout > now){
#ifdef WIN32
        Sleep(1);
#else
        usleep(100);
#endif
		now = dispatch_time(0,0);
	}

   return !(cast_group(group)->count==0);
}

#ifdef __BLOCKS__
void
dispatch_group_notify(dispatch_group_t dg, dispatch_queue_t dq, dispatch_block_t db)
{
    dispatch_group_notify_f(dg, dq, _dispatch_Block_copy(db), _dispatch_call_block_and_release);
}
#endif

void
dispatch_group_notify_f(dispatch_group_t group, dispatch_queue_t queue, void *context, dispatch_function_t work){
    if(!group)
        return;

    cast_group(group)->data = context;
    cast_group(group)->notify = work;
    cast_group(group)->queue = queue;
    if(cast_group(group)->count==0 && work && queue)
        dispatch_async_f(queue, context, work);
}

void
dispatch_group_enter(dispatch_group_t group){
    if(!group)
        return;

    atomic_inc_get(&cast_group(group)->count);
    dispatch_retain(group);
}

void
dispatch_group_leave(dispatch_group_t group){
    if(!group)
        return;

    if(atomic_dec_get(&cast_group(group)->count)==0){
        if(cast_group(group)->notify)
            dispatch_async_f(cast_group(group)->queue,cast_group(group)->data,cast_group(group)->notify);
    }
    dispatch_release(group);
}
