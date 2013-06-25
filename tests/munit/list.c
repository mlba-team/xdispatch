/*
* list.c
*
* Copyright (c) 2010 MLBA.
* All rights reserved.
*
* @LICENSE_HEADER_START@
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
* @LICENSE_HEADER_END@
*/

#include <stdlib.h>

#include "list.h"

item_t * createList()
{
    item_t *t = malloc( sizeof( item_t ) );

    t->data = NULL;
    t->next = NULL;
    return t;
}


int append(
    item_t *lst,
    void *data
)
{
    item_t *neu, *last;

    last = lst;

    while( last->next != NULL )
        last = last->next;

    neu = malloc( sizeof( item_t ) );

    if( neu == NULL )
        return -1;

    last->next = neu;
    neu->data = data;
    neu->next = NULL;
    return 0;
} /* append */


void destroyList(
    item_t *lst
)
{
    if( lst->next != NULL )
        destroyList( lst->next );

    free( lst );
}