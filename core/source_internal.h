/*
* Copyright (c) 2008-2009 Apple Inc. All rights reserved.
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

#ifndef __DISPATCH_SOURCE_INTERNAL__
#define __DISPATCH_SOURCE_INTERNAL__

void _dispatch_source_xref_release(struct dispatch_source_s* ds);
struct dispatch_queue_s* _dispatch_source_invoke(struct dispatch_source_s* ds);
bool _dispatch_source_probe(struct dispatch_source_s*  ds);
void _dispatch_source_dispose(struct dispatch_source_s*  ds);
size_t _dispatch_source_debug(struct dispatch_source_s*  ds, char* buf, size_t bufsiz);

void _dispatch_source_kevent_resume(struct dispatch_source_s* ds, uint32_t new_flags, uint32_t del_flags);
void _dispatch_kevent_merge(struct dispatch_source_s* ds);
void _dispatch_kevent_release(struct dispatch_source_s* ds);
void _dispatch_timer_list_update(struct dispatch_source_s* ds);

#define DSF_CANCELED 1u // cancellation has been requested

void _dispatch_run_timers(void);
// Returns howsoon with updated time value, or NULL if no timers active.
struct timespec *_dispatch_get_next_timer_fire(struct timespec *howsoon);

struct dispatch_kevent_s;
typedef struct dispatch_kevent_s *dispatch_kevent_t;

struct dispatch_timer_source_s {
    uint64_t target;
    uint64_t start;
    uint64_t interval;
    uint64_t leeway;
    uint64_t flags; // dispatch_timer_flags_t
};

struct dispatch_set_timer_params {
    dispatch_source_t ds;
    uintptr_t ident;
    struct dispatch_timer_source_s values;
};

struct dispatch_source_s {
    DISPATCH_STRUCT_HEADER(dispatch_source_s, dispatch_source_vtable_s);
    DISPATCH_QUEUE_HEADER;
    // Instruments always copies DISPATCH_QUEUE_MIN_LABEL_SIZE, which is 64,
    // so the remainder of the structure must be big enough
    union {
        char _ds_pad[DISPATCH_QUEUE_MIN_LABEL_SIZE];
        struct {
            char dq_label[8];
            dispatch_kevent_t ds_dkev;

            dispatch_function_t ds_handler_func;
            void *ds_handler_ctxt;

            void *ds_cancel_handler;

            unsigned int ds_is_level:1,
                ds_is_adder:1,
                ds_is_installed:1,
                ds_needs_rearm:1,
                ds_is_armed:1,
                ds_is_legacy:1,
                ds_cancel_is_block:1,
                ds_handler_is_block:1;

            unsigned int ds_atomic_flags;

            unsigned long ds_data;
            unsigned long ds_pending_data;
            unsigned long ds_pending_data_mask;

            TAILQ_ENTRY(dispatch_source_s) ds_list;

            unsigned long ds_ident_hack;

            struct dispatch_timer_source_s ds_timer;
        };
    };
};

struct dispatch_source_type_s {
    void *opaque;
    uint64_t mask;
    bool (*init) (struct dispatch_source_s* ds,
                  dispatch_source_type_t type,
                  uintptr_t handle,
                  unsigned long mask,
                  dispatch_queue_t q);
};

struct dispatch_source_vtable_s {
    DISPATCH_VTABLE_HEADER(dispatch_source_s);
};


#define DISPATCH_TIMER_INDEX_WALL 0
#define DISPATCH_TIMER_INDEX_MACH 1

#ifdef DISPATCH_NO_LEGACY
enum {
    DISPATCH_TIMER_WALL_CLOCK       = 0x4,
};
enum {
    DISPATCH_TIMER_INTERVAL = 0x0,
    DISPATCH_TIMER_ONESHOT  = 0x1,
    DISPATCH_TIMER_ABSOLUTE = 0x3,
};
enum {
    DISPATCH_MACHPORT_DEAD = 0x1,
    DISPATCH_MACHPORT_RECV = 0x2,
    DISPATCH_MACHPORT_DELETED = 0x4,
};
#endif

#endif /* __DISPATCH_SOURCE_INTERNAL__ */
