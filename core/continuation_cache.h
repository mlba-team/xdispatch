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

#ifndef __CONTINUATION_CACHE_H_
#define __CONTINUATION_CACHE_H_

// If dc_vtable is less than 127, then the object is a continuation.
// Otherwise, the object has a private layout and memory management rules. The
// first two words must align with normal objects.
#define DISPATCH_CONTINUATION_HEADER(x)	\
    const void *				do_vtable;	\
    struct x *volatile	do_next;	\
    dispatch_function_t	dc_func;	\
    void *						dc_ctxt

struct dispatch_continuation_s {
    DISPATCH_CONTINUATION_HEADER(dispatch_continuation_s);
    dispatch_group_t	dc_group;
    void *				dc_data[3];
};

typedef struct dispatch_continuation_s *dispatch_continuation_t;

extern malloc_zone_t *_dispatch_ccache_zone;
dispatch_continuation_t _dispatch_continuation_alloc_from_heap(void);

static inline dispatch_continuation_t
_dispatch_continuation_alloc_cacheonly(void)
{
    dispatch_continuation_t dc = fastpath(_dispatch_thread_getspecific(dispatch_cache_key));
    if (dc) {
        _dispatch_thread_setspecific(dispatch_cache_key, dc->do_next);
    }
    return dc;
}

#endif /* __CONTINUATION_CACHE_H_ */
