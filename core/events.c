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

void _evt_init(_evt_loop_t t, _evt_handler h){
	assert(t);
	assert(h);
	t->cb = h;
	pthread_mutex_init(&t->access, NULL);
	pthread_cond_init(&t->cond, NULL);
	t->used = TRUE;
}

_evt_loop_t _evt_create(_evt_handler h){
	_evt_loop_t neu = (_evt_loop_t)malloc(sizeof(struct _evt_loop_s));
	assert(neu);
	memset(neu,0,sizeof(struct _evt_loop_s));

	assert(h);

	_evt_init(neu, h);

	return neu;
}

int _evt_signal(_evt_loop_t l, int type, void* data){
	// create new item
	_evt_event_t i = (_evt_event_t)malloc(sizeof(struct _evt_event_s));

	assert(l);

	if (i == NULL)
		return -1; // creation failed
	i->type = type; // link with the given event type
	i->data = data;
	i->next = NULL;

	pthread_mutex_lock(&l->access);
	if (l->last != NULL)
		l->last->next = i;
	l->last = i;
	if (l->first == NULL) { // if this item is first one, signal waiting getitem call
		l->first = i;
		pthread_cond_broadcast(&l->cond);
	}
	pthread_mutex_unlock(&l->access);
	return 0;
}

void _evt_run(_evt_loop_t l, int flags){
	_evt_event_t curr = NULL;

	assert(l);
        if(!l){
            printf("foo");
        }

	pthread_mutex_lock(&l->access);
	while(1){
		// get item from list, if available
		if (l->first == NULL) {
			if (flags == EVT_NOWAIT) { // non-blocking call
				pthread_mutex_unlock(&l->access);
				return;
			} else {
				// EVT_NONE - blocking call
				pthread_cond_wait(&l->cond, &l->access); // no item available, wait for it
				continue;
			}
		}
		curr = l->first;
		l->first = curr->next; // remove item
		if (l->first == NULL)
			l->last = NULL; // this was the last item
		pthread_mutex_unlock(&l->access);

		assert(curr);

		if(curr->type != EVT_EXIT) {
			// pass the event to the loop's callback
			l->cb(l,curr->type, curr->data);
			free(curr); // delete item
		} else {
			// the exit event was passed
			free(curr);
			break;
		}

		pthread_mutex_lock(&l->access);
	}

}

void _evt_destroy(_evt_loop_t l){
	_evt_event_t i = NULL, curr = NULL;

	assert(l);

	if (l->used == FALSE)
		return;

	// delete all waiting items
	i = l->first;
	while (i != NULL) {
		curr = i;
		i = i->next;
		free(curr);
	}

	// reset list
	l->first = NULL;
	l->last = NULL;

	pthread_mutex_destroy(&l->access);
	pthread_cond_destroy(&l->cond);

	free(l);
}
