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


static void
evfilt_mux_demux(struct knote* kn)
{

  if (kn->kev.fflags & NOTE_MUX_READABLE) {
    if (kn->kn_mux_kn_read) {
      if (!(kn->kn_mux_kn_read->kn_flags & KNFL_KNOTE_DELETED)) {
        struct filter *filt = knote_get_filter(kn->kn_mux_kn_read);
        dbg_printf("readable mux %p calling read knote %p", kn, kn->kn_mux_kn_read);
        filt->kf_rw_callback(kn->kn_mux_kn_read);
        kn->kev.fflags &= ~NOTE_MUX_READABLE;
      } else {
        dbg_printf("readable mux %p not calling deleted read knote %p", kn, kn->kn_mux_kn_read);
      }
    } else {
      dbg_printf("readable mux %p has no read knote", kn);
    }
  } 
  if (kn->kev.fflags & NOTE_MUX_WRITABLE) {
    if (kn->kn_mux_kn_write) {
      if (!(kn->kn_mux_kn_write->kn_flags & KNFL_KNOTE_DELETED)) {
        struct filter *filt = knote_get_filter(kn->kn_mux_kn_write);
        dbg_printf("writable mux %p calling write knote %p", kn, kn->kn_mux_kn_read);
        filt->kf_rw_callback(kn->kn_mux_kn_write);
        kn->kev.fflags &= ~NOTE_MUX_WRITABLE;
      } else {
        dbg_printf("writable mux %p not calling deleted write knote %p", kn, kn->kn_mux_kn_read);
      }
    } else {
      dbg_printf("writable mux %p has no write knote", kn);
    }
  }
}

static VOID CALLBACK
evfilt_mux_callback(void *param, BOOLEAN fired)
{
    WSANETWORKEVENTS events;
    struct kqueue *kq;
    struct knote *kn;
    int rv;

    assert(param);

    if (fired) {
        dbg_puts("called, but event was not triggered(?)");
        return;
    }
    
    assert(param);
    kn = (struct knote*)param;
    // FIXME: check if knote is pending destroyed
    kq = kn->kn_kq;
    assert(kq);

    memset(&events, 0, sizeof(events));
    /* Retrieve the socket events and update the knote */
    rv = WSAEnumNetworkEvents(
            (SOCKET) kn->kev.ident, 
            kn->data.handle,
                &events);
    if (rv != 0) {
        dbg_wsalasterror("WSAEnumNetworkEvents");
        return; //fIXME: should crash or invalidate the knote
    }

    if (events.lNetworkEvents & (FD_ACCEPT | FD_READ)) {
      dbg_printf("MUX knote %p got readable callback", kn);
      kn->kev.fflags |= NOTE_MUX_READABLE;
    }
    if (events.lNetworkEvents & (FD_WRITE)) {
      dbg_printf("MUX knote %p got writable callback", kn);
      kn->kev.fflags |= NOTE_MUX_WRITABLE;
    }
    evfilt_mux_demux(kn);
}

int
evfilt_mux_copyout(struct kevent *dst, struct knote *src, void *ptr)
{
  assert(0 && "MUX copyout unused");
}

int
evfilt_mux_knote_create(struct filter *filt, struct knote *kn)
{
    HANDLE evt;
    int rv;

    if (windows_get_descriptor_type(kn) < 0)
            return (-1);

    /* Create an auto-reset event object */
    evt = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (evt == NULL) {
        dbg_lasterror("CreateEvent()");
        return (-1);
    }

    rv = WSAEventSelect(
                (SOCKET) kn->kev.ident, 
                evt, 
                FD_READ | FD_WRITE | FD_ACCEPT | FD_CLOSE);
    if (rv != 0) {
        dbg_wsalasterror("WSAEventSelect()");
        CloseHandle(evt);
        return (-1);
    }
    
    /* TODO: handle regular files in addition to sockets */

    kn->data.handle = evt;

    if (RegisterWaitForSingleObject(&kn->kn_event_whandle, evt, 
	    evfilt_mux_callback, kn, INFINITE, 0) == 0) {
        dbg_puts("RegisterWaitForSingleObject failed");
        CloseHandle(evt);
        return (-1);
    }

    knote_retain(kn); // OS knows about this knote, keep it around

    return (0);
}

int
evfilt_mux_knote_modify(struct filter *filt, struct knote *kn, 
        const struct kevent *kev)
{
  unsigned int demux = 0;
  int rv = 0;
  if (kev->fflags & NOTE_MUX_READ) {
    if (kn->kev.fflags & NOTE_MUX_READ) {
      // we're all set
    } else {
      // enable read
      kn->kev.fflags |= NOTE_MUX_READ;
      // do we have to tell anyone
      demux |= kn->kev.fflags & NOTE_MUX_READABLE;
      recv(kn->kev.ident, 0, 0, 0);
    }
  } else {
    // disable read
    kn->kev.fflags &= ~NOTE_MUX_READ;
    // but don't throw away old readable state
  }
  if (kev->fflags & NOTE_MUX_WRITE) {
    if (kn->kev.fflags & NOTE_MUX_WRITE) {
      // we're all set
    } else {
      // enable write
      kn->kev.fflags |= NOTE_MUX_WRITE;
      // do we have to tell anyone
      demux |= kn->kev.fflags & NOTE_MUX_WRITABLE;
    }
  } else {
    // disable write
    kn->kev.fflags &= ~NOTE_MUX_WRITE;
    // but don't throw away old writeable state
  }
  return (0);
}

int
evfilt_mux_knote_delete(struct filter *filt, struct knote *kn)
{
    if (kn->data.handle == NULL || kn->kn_event_whandle == NULL)
        return (0);

	if(!UnregisterWaitEx(kn->kn_event_whandle, INVALID_HANDLE_VALUE)) {
		dbg_lasterror("UnregisterWait()");
		return (-1);
	}
	if (!WSACloseEvent(kn->data.handle)) {
		dbg_wsalasterror("WSACloseEvent()");
		return (-1);
	}

    kn->data.handle = NULL;
    knote_release(kn);
    return (0);
}

int
evfilt_mux_knote_enable(struct filter *filt, struct knote *kn)
{
  assert(0 && "MUX enable unused");
}

int
evfilt_mux_knote_disable(struct filter *filt, struct knote *kn)
{
  assert(0 && "MUX disable unused");
}

const struct filter evfilt_mux = {
    42,
    NULL,
    NULL,
    evfilt_mux_copyout,
    evfilt_mux_knote_create,
    evfilt_mux_knote_modify,
    evfilt_mux_knote_delete,
    evfilt_mux_knote_enable,
    evfilt_mux_knote_disable,         
};
