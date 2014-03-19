/*
 * Copyright (c) 2008-2009 Apple Inc. All rights reserved.
 *
 * @APPLE_APACHE_LICENSE_HEADER_START@
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
 * @APPLE_APACHE_LICENSE_HEADER_END@
 */

#include "internal.h"
#include "kevent_internal.h"

#if defined(_MSC_VER)
#pragma comment(lib, "ws2_32.lib")
#endif

static bool _dispatch_select_workaround = false;
static fd_set _dispatch_rfds;
static fd_set _dispatch_wfds;
static void *_dispatch_rfd_ptrs[FD_SETSIZE];
static void *_dispatch_wfd_ptrs[FD_SETSIZE];

static void
_dispatch_get_kq_init(void *context)
{
        int *_dispatch_kqp = (int*)context;
	static const struct kevent kev = {
		1,                /* .ident */
		EVFILT_USER,      /* .filter */
		EV_ADD|EV_CLEAR,  /* .flags */
	};

	*_dispatch_kqp = kqueue();
	_dispatch_safe_fork = false;
	// in case we fall back to select()
        FD_SET(*_dispatch_kqp, &_dispatch_rfds);

	if (*_dispatch_kqp == -1) {
		dispatch_assert_zero(errno);
	}

#if defined(__GNUC__)
	(void)
#endif
    dispatch_assume_zero(kevent(*_dispatch_kqp, &kev, 1, NULL, 0, NULL));
    _dispatch_queue_push(_dispatch_mgr_q.do_targetq, (struct dispatch_object_s*)&_dispatch_mgr_q);
}

static int
_dispatch_get_kq(void)
{
	static dispatch_once_t pred;
        static int _dispatch_kq;

	dispatch_once_f(&pred, &_dispatch_kq, _dispatch_get_kq_init);

	return _dispatch_kq;
}

static void
_dispatch_mgr_thread2(struct kevent *kev, size_t cnt)
{
	size_t i;

	for (i = 0; i < cnt; i++) {
		// EVFILT_USER isn't used by sources
		if (kev[i].filter == EVFILT_USER) {
			// If _dispatch_mgr_thread2() ever is changed to return to the
			// caller, then this should become _dispatch_queue_drain()
			_dispatch_queue_serial_drain_till_empty(&_dispatch_mgr_q);
		} else {
			_dispatch_source_drain_kevent(&kev[i]);
		}
    }
}

static dispatch_queue_t
_dispatch_mgr_invoke(dispatch_queue_t dq)
{
	static const struct timespec timeout_immediately = { 0, 0 };
	struct timespec timeout;
	const struct timespec *timeoutp;
	struct timeval sel_timeout, *sel_timeoutp;
	fd_set tmp_rfds, tmp_wfds;
	struct kevent kev[1];
	int k_cnt, k_err, i, r;
        int _dispatch_kq = _dispatch_get_kq();

	_dispatch_thread_setspecific(dispatch_queue_key, dq);

	for (;;) {
		_dispatch_run_timers();

		timeoutp = _dispatch_get_next_timer_fire(&timeout);
		
		if (_dispatch_select_workaround) {
			FD_COPY(&_dispatch_rfds, &tmp_rfds);
			FD_COPY(&_dispatch_wfds, &tmp_wfds);
			if (timeoutp) {
				sel_timeout.tv_sec = (long)timeoutp->tv_sec;
#if defined(__GNUC__)
				sel_timeout.tv_usec = (typeof(sel_timeout.tv_usec))(timeoutp->tv_nsec / 1000u);
#else
            /* Fragile! */
				sel_timeout.tv_usec = (long)(timeoutp->tv_nsec / 1000u);
#endif
				sel_timeoutp = &sel_timeout;
			} else {
				sel_timeoutp = NULL;
			}
			
			r = select(FD_SETSIZE, &tmp_rfds, &tmp_wfds, NULL, sel_timeoutp);
			if (r == -1) {
				if (errno != EBADF) {
#if defined(__GNUC__)
					(void)
#endif
					dispatch_assume_zero(errno);
					continue;
				}
				for (i = 0; i < FD_SETSIZE; i++) {
					if (i == _dispatch_kq) {
						continue;
					}
					if (!FD_ISSET(i, &_dispatch_rfds) && !FD_ISSET(i, &_dispatch_wfds)) {
						continue;
					}
					r = dup(i);
					if (r != -1) {
						close(r);
					} else {
						FD_CLR(i, &_dispatch_rfds);
						FD_CLR(i, &_dispatch_wfds);
						_dispatch_rfd_ptrs[i] = 0;
						_dispatch_wfd_ptrs[i] = 0;
					}
				}
				continue;
			}
			
			if (r > 0) {
				for (i = 0; i < FD_SETSIZE; i++) {
					if (i == _dispatch_kq) {
						continue;
					}
					if (FD_ISSET(i, &tmp_rfds)) {
						FD_CLR(i, &_dispatch_rfds);	// emulate EV_DISABLE
						EV_SET(&kev[0], i, EVFILT_READ, EV_ADD|EV_ENABLE|EV_DISPATCH, 0, 1, _dispatch_rfd_ptrs[i]);
						_dispatch_rfd_ptrs[i] = 0;
						_dispatch_mgr_thread2(kev, 1);
					}
					if (FD_ISSET(i, &tmp_wfds)) {
						FD_CLR(i, &_dispatch_wfds);	// emulate EV_DISABLE
						EV_SET(&kev[0], i, EVFILT_WRITE, EV_ADD|EV_ENABLE|EV_DISPATCH, 0, 1, _dispatch_wfd_ptrs[i]);
						_dispatch_wfd_ptrs[i] = 0;
						_dispatch_mgr_thread2(kev, 1);
					}
				}
			}
			
			timeoutp = &timeout_immediately;
		}
		
		k_cnt = kevent(_dispatch_kq, NULL, 0, kev, sizeof(kev) / sizeof(kev[0]), timeoutp);
		k_err = errno;

		switch (k_cnt) {
		case -1: 
			{
				if (k_err == EBADF) {
					DISPATCH_CLIENT_CRASH("Do not close random Unix descriptors");
				}
#if defined(__GNUC__)
				(void)
#endif
				dispatch_assume_zero(k_err);
			}
            break;
		case 0:
			_dispatch_force_cache_cleanup();
			break;
#if defined(__GNUC__)
			(void)
#endif
			dispatch_assume_zero(k_err);
			continue;
        default:
			_dispatch_mgr_thread2(kev, (size_t)k_cnt);
			_dispatch_force_cache_cleanup();
		}
	}

	return NULL;
}

static bool
_dispatch_mgr_wakeup(dispatch_queue_t dq)
{
	static const struct kevent kev = {
		1,                /* .ident */
		EVFILT_USER,      /* .filter */
		0,                /* .flags */
		NOTE_TRIGGER,     /* .fflags */
	};

	_dispatch_debug("waking up the _dispatch_mgr_q: %p", dq);

	_dispatch_update_kq(&kev);

	return false;
}

void
_dispatch_update_kq(const struct kevent *kev)
{
	struct kevent kev_copy = *kev;
	int rval = 0;
	
	kev_copy.flags |= EV_RECEIPT;

	if (kev_copy.flags & EV_DELETE) {
		switch (kev_copy.filter) {
		case EVFILT_READ:
			if (FD_ISSET((int)kev_copy.ident, &_dispatch_rfds)) {
				FD_CLR((int)kev_copy.ident, &_dispatch_rfds);
				_dispatch_rfd_ptrs[kev_copy.ident] = 0;
				return;
			}
		case EVFILT_WRITE:
			if (FD_ISSET((int)kev_copy.ident, &_dispatch_wfds)) {
				FD_CLR((int)kev_copy.ident, &_dispatch_wfds);
				_dispatch_wfd_ptrs[kev_copy.ident] = 0;
				return;
			}
		default:
			break;
		}
	}
	
	rval = kevent(_dispatch_get_kq(), &kev_copy, 1, &kev_copy, 1, NULL);

	if (rval == -1) { 
		// If we fail to register with kevents, for other reasons aside from
		// changelist elements.
#if defined(__GNUC__)
		(void)
#endif
		dispatch_assume_zero(errno);
		//kev_copy.flags |= EV_ERROR;
		//kev_copy.data = error;
		return;
	}

	// The following select workaround only applies to adding kevents
	if (!(kev->flags & EV_ADD)) {
		return;
	}

	switch (kev_copy.data) {
	case 0:
		return;
	case EBADF:
		break;
	default:
		// If an error occurred while registering with kevent, and it was 
		// because of a kevent changelist processing && the kevent involved
		// either doing a read or write, it would indicate we were trying
		// to register a /dev/* port; fall back to select
		switch (kev_copy.filter) {
		case EVFILT_READ:
			_dispatch_select_workaround = true;
			FD_SET((int)kev_copy.ident, &_dispatch_rfds);
			_dispatch_rfd_ptrs[kev_copy.ident] = kev_copy.udata;
			break;
		case EVFILT_WRITE:
			_dispatch_select_workaround = true;
			FD_SET((int)kev_copy.ident, &_dispatch_wfds);
			_dispatch_wfd_ptrs[kev_copy.ident] = kev_copy.udata;
			break;
		default:
			_dispatch_source_drain_kevent(&kev_copy); 
			break;
		}
		break;
	}
}

static const struct dispatch_queue_vtable_s _dispatch_queue_mgr_vtable = {
	DISPATCH_QUEUE_MGR_TYPE,                                 /* .do_type */
	"mgr-queue",                                             /* .do_kind */
	dispatch_queue_debug,                                    /* .do_debug */
	_dispatch_mgr_invoke,                                    /* .do_invoke */
	_dispatch_mgr_wakeup,                                    /* .do_probe */
};

// 6618342 Contact the team that owns the Instrument DTrace probe before renaming this symbol
struct dispatch_queue_s _dispatch_mgr_q = {
	&_dispatch_queue_mgr_vtable,                             /* .do_vtable */
	0,                                                       /* .do_next */
	DISPATCH_OBJECT_GLOBAL_REFCNT,                           /* .do_ref_cnt */
	DISPATCH_OBJECT_GLOBAL_REFCNT,                           /* .do_xref_cnt */
	DISPATCH_OBJECT_SUSPEND_LOCK,                            /* .do_suspend_cnt */
	&_dispatch_root_queues[DISPATCH_ROOT_QUEUE_COUNT - 1],   /* .do_targetq */
	0,                                                       /* .do_ctxt */

	0,                                                       /* .do_finalizer */

	0,                                                       /* .dq_running */
	1,                                                       /* .dq_width */
	0,                                                       /* .dq_items_tail */
	0,                                                       /* .dq_items_head */
	2,                                                       /* .dq_serialnum */
	0,                                                       /* .dq_finalizer_ctxt */
	"com.apple.libdispatch-manager",                         /* .dq_label */
};

