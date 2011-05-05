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


#include "internal.h"

malloc_zone_t *_dispatch_ccache_zone;

void
_dispatch_ccache_init(void *context DISPATCH_UNUSED)
{
	_dispatch_ccache_zone = malloc_create_zone(0, 0);
	dispatch_assert(_dispatch_ccache_zone);
	malloc_set_zone_name(_dispatch_ccache_zone, "DispatchContinuations");
}

dispatch_continuation_t
_dispatch_continuation_alloc_from_heap(void)
{
	static dispatch_once_t pred;
	dispatch_continuation_t dc;

	dispatch_once_f(&pred, NULL, _dispatch_ccache_init);

	while (!(dc = (dispatch_continuation_t)(dispatch_continuation_t)fastpath(malloc_zone_calloc(_dispatch_ccache_zone, 1, ROUND_UP_TO_CACHELINE_SIZE(sizeof(*dc)))))) {
		sleep(1);
	}

	return dc;
}

void
_dispatch_force_cache_cleanup(void)
{
	dispatch_continuation_t dc = (dispatch_continuation_t)_dispatch_thread_getspecific(dispatch_cache_key);
	if (dc) {
		_dispatch_thread_setspecific(dispatch_cache_key, NULL);
		_dispatch_cache_cleanup2(dc);
	}
}

DISPATCH_NOINLINE
void
_dispatch_cache_cleanup2(void *value)
{
	dispatch_continuation_t dc, next_dc = (dispatch_continuation_t)value;

	while ((dc = next_dc)) {
		next_dc = dc->do_next;
		malloc_zone_free(_dispatch_ccache_zone, dc);
	}
}
