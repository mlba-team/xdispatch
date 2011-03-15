/*
' Copyright (c) 2008-2009 Apple Inc. All rights reserved.
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

void
	dispatch_debug(dispatch_object_t object, const char *message, ...){
		if(!object)
			return;

		printf("dispatch_object_t:: references: %u | ", object->count);

		switch(object->type){
		case DISPATCH_QUEUE:
		case DISPATCH_SERIAL_QUEUE:
			printf("dispatch_queue: %s | %i items | Message: %s\n", cast_queue(object)->name, _tq_count(cast_queue(object)), message);
			break;
		case DISPATCH_GROUP:
			printf("dispatch_group: %i pending | Message: %s\n", cast_group(object)->count, message);
			break;
		default:
			printf("%s", message);
		}
}

void
	dispatch_debugv(dispatch_object_t object, const char *message, va_list ap){
		if(!object)
			return;

		printf("references: %u | ", object->count);

		switch(object->type){
		case DISPATCH_QUEUE:
		case DISPATCH_SERIAL_QUEUE:
			printf("dispatch_queue: %s | ", cast_queue(object)->name);
			break;
		case DISPATCH_GROUP:
			printf("dispatch_group: %i pending | ", cast_group(object)->count);
			break;
		default:
			break;
		}
		printf(message,ap);
}

void
	dispatch_retain(dispatch_object_t object){
		atomic_inc_get(&object->count);
}


/**
Invoked whenever an object is distroyed 
that had a target queue associated with it
*/
void _finalize(void* context){
	dispatch_object_t o = (dispatch_object_t)(context);

	if(!context)
		return;

	if(o->finalizer)
		o->finalizer(o->context);
	_destroy_object(o);
}

void
	dispatch_release(dispatch_object_t object){
		int ct = atomic_dec_get(&object->count);
		if (ct == 0) {
			if(object->target)
				dispatch_async_f((dispatch_queue_t)object->target, object, _finalize);
			else {
				_finalize(object);
			}
		}
}

void *
	dispatch_get_context(dispatch_object_t object){
		if(!object)
			return NULL;
		else
			return object->context;
}

void
	dispatch_set_context(dispatch_object_t object, void *context){
		if(!object)
			return;

		object->context = context;
}

void
	dispatch_set_finalizer_f(dispatch_object_t object, dispatch_function_t finalizer){
		if(!object)
			return;

		object->finalizer = finalizer;
}

void
	dispatch_suspend(dispatch_object_t object){
		if(!object)
			return;

		object->suspended = TRUE;
}

void
	dispatch_resume(dispatch_object_t object){
		if(!object)
			return;

		object->suspended = FALSE;
		//_tp_signal(
}