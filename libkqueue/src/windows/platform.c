/*
 * Copyright (c) 2011 Mark Heily <mark@heily.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "../common/private.h"

struct event_buf {
    DWORD       bytes;
    ULONG_PTR   key;
    OVERLAPPED *overlap;
};

#if NO_IMPLICIT_TLS_WORKAROUND

/*
 * Per-thread evt event buffer used to ferry data between
 * kevent_wait() and kevent_copyout().
 */
static __thread struct event_buf iocp_buf;

#else

/*
 * Per-thread evt event buffer used to ferry data between
 * kevent_wait() and kevent_copyout().
 */
#define iocp_buf (*_iocp_buf())
static DWORD event_buf_tls = 0;
static int event_buf_tls_init = 0;


// workaround for implicit TLS initialization
// bug on Windows prior to Windows Vista
struct event_buf* _libkqueue_thread_attach(){
  struct event_buf* ev_buf = malloc(sizeof(struct event_buf));
  assert(ev_buf);
  TlsSetValue(event_buf_tls, ev_buf);
  return ev_buf;
}

void libkqueue_thread_attach(){
  _libkqueue_thread_attach();
}

struct event_buf *_iocp_buf() {
  if (!event_buf_tls_init)
    libkqueue_process_attach();
  struct event_buf* ev_buf = ((struct event_buf*)TlsGetValue(event_buf_tls));
  if (!ev_buf) {
    dbg_puts("AAARrrr, no iocp_buf? fixing up missing libkqueue_thread_attach");
    ev_buf = _libkqueue_thread_attach();
  }
  return ev_buf;
}

void libkqueue_thread_detach(){
	struct event_buf* ev_buf = TlsGetValue(event_buf_tls);
	assert(ev_buf);
	free(ev_buf);
}

void libkqueue_process_attach(){
	event_buf_tls = TlsAlloc();
        event_buf_tls_init = 1;
	libkqueue_thread_attach();
}

#endif


/* FIXME: remove these as filters are implemented */
const struct filter evfilt_proc = EVFILT_NOTIMPL;
const struct filter evfilt_vnode = EVFILT_NOTIMPL;
const struct filter evfilt_signal = EVFILT_NOTIMPL;

const struct kqueue_vtable kqops = {
    windows_kqueue_init,
    windows_kqueue_free,
    windows_kevent_wait,
    windows_kevent_copyout,
    windows_filter_init,
    windows_filter_free,
};

#ifndef MAKE_STATIC

BOOL WINAPI DllMain(
        HINSTANCE self,
        DWORD reason,
        LPVOID unused)
{
    switch (reason) { 
        case DLL_PROCESS_ATTACH:

#if XXX
			//move to EVFILT_READ?
            if (WSAStartup(MAKEWORD(2,2), NULL) != 0)
                return (FALSE);
#endif
            libkqueue_init();
            break;

        case DLL_PROCESS_DETACH:
#if XXX
            WSACleanup();
#endif
            break;
    }

    return (TRUE);
}

#endif

int
windows_kqueue_init(struct kqueue *kq)
{
    kq->kq_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 
                                         (ULONG_PTR) 0, 0);
    if (kq->kq_iocp == NULL) {
        dbg_lasterror("CreateIoCompletionPort");
        return (-1);
    }

#if DEADWOOD
    /* Create a handle whose sole purpose is to indicate a synthetic
     * IO event. */
    kq->kq_synthetic_event = CreateSemaphore(NULL, 0, 1, NULL);
    if (kq->kq_synthetic_event == NULL) {
        /* FIXME: close kq_iocp */
        dbg_lasterror("CreateSemaphore");
        return (-1);
    }

    kq->kq_loop = evt_create();
    if (kq->kq_loop == NULL) {
        dbg_perror("evt_create()");
        return (-1);
    }
#endif

	if(filter_register_all(kq) < 0) {
		CloseHandle(kq->kq_iocp);
		return (-1);
	}

    return (0);
}

int
windows_kqueue_post(struct kqueue *kq, struct knote *kn)
{
  knote_retain(kn); // this pairs with windows_kevent_copyout
  if (!PostQueuedCompletionStatus(kq->kq_iocp, 1, (ULONG_PTR)0, (LPOVERLAPPED)kn)) {
    dbg_lasterror("PostQueuedCompletionStatus()");
    knote_release(kn);
    return -1;
    /* FIXME: need more extreme action */
  }
  return 0;
}

void
windows_kqueue_free(struct kqueue *kq)
{
    CloseHandle(kq->kq_iocp);
    free(kq);
}

int
windows_kevent_wait(struct kqueue *kq, int no, const struct timespec *timeout)
{
	int retval;
    DWORD       timeout_ms;
    BOOL        success;
    
    if (timeout == NULL) {
        timeout_ms = INFINITE;
    } else if ( timeout->tv_sec == 0 && timeout->tv_nsec < 1000000 ) {
        /* do we need to try high precision timing? */
        // TODO: This is currently not possible on windows!
        timeout_ms = 0;
    } else {  /* Convert timeout to milliseconds */
        timeout_ms = 0;
        if (timeout->tv_sec > 0)
            timeout_ms += ((DWORD)timeout->tv_sec) * 1000;
        if (timeout->tv_nsec > 0)
            timeout_ms += timeout->tv_nsec / 1000000;
    }

    dbg_printf("waiting for events (timeout=%u ms)", (unsigned int) timeout_ms);
#if 0
    if(timeout_ms <= 0)
        dbg_printf("Woop, not waiting !?");
#endif        
    memset(&iocp_buf, 0, sizeof(iocp_buf));
    success = GetQueuedCompletionStatus(kq->kq_iocp, 
            &iocp_buf.bytes, &iocp_buf.key, &iocp_buf.overlap, 
            timeout_ms);
    if (success) {
      dbg_printf("======= dequeued knote %p", iocp_buf.overlap);
        return (1);
    } else {
        if (GetLastError() == WAIT_TIMEOUT) {
            dbg_puts("no events within the given timeout");
            return (0);
        }
        dbg_lasterror("GetQueuedCompletionStatus");
        return (-1);
    }

    return (retval);
}

int
windows_kevent_copyout(struct kqueue *kq, int nready,
        struct kevent *eventlist, int nevents)
{
    struct filter *filt;
	struct knote* kn;
    int rv, nret;

    //FIXME: not true for EVFILT_IOCP
    kn = (struct knote *) iocp_buf.overlap;
    assert(kn);
    filt = &kq->kq_filt[~(kn->kev.filter)];
    rv = filt->kf_copyout(eventlist, kn, &iocp_buf);
    if (slowpath(rv < 0)) {
        dbg_puts("knote_copyout failed");
        /* XXX-FIXME: hard to handle this without losing events */
        abort();
    } else {
        nret = 1;
    }
    knote_release(kn); // this pairs with windows_kqueue_post()

    /*
     * Certain flags cause the associated knote to be deleted
     * or disabled.
     */
    if (eventlist->flags & EV_DISPATCH) 
        knote_disable(filt, kn); //TODO: Error checking
    if (eventlist->flags & EV_ONESHOT)
        knote_delete(filt, kn); //TODO: Error checking

    /* If an empty kevent structure is returned, the event is discarded. */
    if (fastpath(eventlist->filter != 0)) {
        eventlist++;
    } else {
        dbg_puts("spurious wakeup, discarding event");
        nret--;
    }

	return nret;
}

int
windows_filter_init(struct kqueue *kq, struct filter *kf)
{

	kq->kq_filt_ref[kq->kq_filt_count] = (struct filter *) kf;
    kq->kq_filt_count++;

	return (0);
}

void
windows_filter_free(struct kqueue *kq, struct filter *kf)
{

}

int
windows_get_descriptor_type(struct knote *kn)
{
    socklen_t slen;
    struct stat sb;
    int i, lsock;

    /*
     * Test if the descriptor is a socket.
     */
	/* Assume that the HANDLE is a socket. */
	/* TODO: we could do a WSAIoctl and check for WSAENOTSOCK */

	/*
	* Test if the socket is active or passive.
	*/
	slen = sizeof(lsock);
	lsock = 0;
	i = getsockopt(kn->kev.ident, SOL_SOCKET, SO_ACCEPTCONN, (char *)&lsock, &slen);
	if (i == 0) {
		// this is a socket
		if (lsock) {
			kn->kn_flags |= KNFL_PASSIVE_SOCKET;
		}
		else {
			dbg_printf("HANDLE %d appears to a be an active socket ???", kn->kev.ident);
			// ???
		}
	} else if (fstat((int)kn->kev.ident, &sb) == 0) {
        dbg_printf("HANDLE %d appears to a be regular file", kn->kev.ident);
        kn->kn_flags |= KNFL_REGULAR_FILE;
    } else {
		// ???
		dbg_printf("HANDLE %d appears to a be neither a file neither a socket???", kn->kev.ident);
	}

    return (0);
}
