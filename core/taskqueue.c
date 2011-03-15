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

struct _taskqueue_s* _tq_create(){
	struct _taskqueue_s* n = NULL;
	n = (struct _taskqueue_s*)malloc(sizeof(struct _taskqueue_s));
	_tq_init(n);
	return n;
}

void _tq_name(_taskqueue_t q, const char* label){
                size_t size = 0;
                char* name = NULL;

                if(!label)
                    return;
                size = strlen(label)+1;
                name = (char*)malloc(size*sizeof(char));
#ifdef WIN32
		strcpy_s(name,size,label);
#else
		strcpy(name,label);
#endif
		q->name = name;
}

void _tq_init(struct _taskqueue_s* n){
	memset(n,0,sizeof(struct _taskqueue_s));
}

void _tq_destroy(struct _taskqueue_s* t){
	_tq_clear(t);
	if(t->name)
		free(t->name);
	free(t);
}

void _tq_clear(struct _taskqueue_s* t){
	_taskitem_t c = NULL;
	_taskitem_t del = NULL;
	TQ_SAFE_ENTER(t);
	c = t->first;
	while(c){
		del = c;
		c = c->next;
		_tq_delete(del);
	}
	TQ_SAFE_LEAVE(t);
}

void _tq_remove(struct _taskqueue_s* t,_taskitem_t i){
	_taskitem_t c = NULL;
	TQ_SAFE_ENTER(t);
	c = t->first;
	if(c==i){
		t->first = NULL;
		TQ_SAFE_LEAVE(t);
		return;
	}
	while(c){
		if(c->next && c->next==i) {
			if(c->next->next == NULL)
				t->last = c;
			c->next = c->next->next;
			TQ_SAFE_LEAVE(t);
			return;
		}
		c=c->next;
	}
	TQ_SAFE_LEAVE(t);
}
