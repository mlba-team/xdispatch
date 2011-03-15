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

_threadpool_t _tp_create(){
	_threadpool_t n = NULL;
	n = (_threadpool_t)malloc(sizeof(struct _threadpool_s));
    memset(n,0,sizeof(struct _threadpool_s));
	_evt_init(&n->loop, _loop_worker);
	return n;
}

void _thread_kill(_thread_t t){

}

_thread_t _thread_create(){
	_thread_t neu = (_thread_t)malloc(sizeof(struct _thread_s));
	memset(neu,0,sizeof(struct _thread_s));

        assert(neu);

	_spawn_thread(neu);

	return neu;
}

void _tp_destroy(_threadpool_t t){
	_thread_t c = NULL;
	_thread_t del = NULL;

	c = t->first;
	while(c){
		del = c;
		c = c->next;
		_thread_kill(c);
	}

	free(t);
}

void _tp_remove(_threadpool_t t, _thread_t i){
	_thread_t c = NULL;
	TP_SAFE_ENTER(t);
	c = t->first;
	if(c==i){
		t->first = NULL;
		TP_SAFE_LEAVE(t);
		_thread_kill(c);
		return;
	}
	while(c){
		if(c->next && c->next==i) {
			c->next = c->next->next;
			TP_SAFE_LEAVE(t);
			_thread_kill(c);
			return;
		}
		c=c->next;
	}
	t->count--;

	TP_SAFE_LEAVE(t);
}

_thread_t _tp_add(_threadpool_t t){
        _thread_t n = NULL;

        n = _thread_create();

	if(!n) {
		printf("Thread creation broken, xdispatch can't continue\n");
		exit(2);
	}

	TP_SAFE_ENTER(t);
        if(t->loop.cb==NULL)
            t->loop.cb = _loop_worker;
	n->loop = &t->loop;
	n->next = t->first;
	t->first = n;
	t->count++;
	TP_SAFE_LEAVE(t);
	return n;
}

void _tp_signal(_threadpool_t p){
        assert(p);
        assert(&(p->loop));
        _evt_signal(&(p->loop), EVT_WAKEUP, 0);
	/*_thread_t t = _tp_first(p);
	while(t){
		_tp_signal__thread(t);
		t = _tp_next(p,t);
	}*/
}

