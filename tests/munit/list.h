/*
* list.h
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

#ifndef LIST_H_
#define LIST_H_

typedef struct s_item_t
{
    struct s_item_t *next;
    void *data;
} item_t;

item_t *
createList();

void
destroyList(
    item_t *lst
);

int
append(
    item_t *lst,
    void *data
);

#endif /* LIST_H_ */