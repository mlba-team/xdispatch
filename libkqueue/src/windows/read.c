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
evfilt_read_callback(struct knote *kn)
{
    struct kqueue *kq;

    assert(kn);
    kq = kn->kn_kq;
    assert(kq);

    if (kn->kn_mux->kev.fflags & NOTE_MUX_READABLE) {
        dbg_printf("==== posting read knote %p", kn);
        if (!windows_kqueue_post(kq, kn)) {
            return;
        }
    } else {
        dbg_printf("==== read knote not readable?? %p", kn);
    }
}

static void
evfilt_write_callback(struct knote *kn)
{
    struct kqueue *kq;

    assert(kn);
    kq = kn->kn_kq;
    assert(kq);

    if (kn->kn_mux->kev.fflags & NOTE_MUX_WRITABLE) {
        dbg_printf("==== posting read knote %p", kn);
        if (!windows_kqueue_post(kq, kn)) {
            return;
        }
    } else {
        dbg_printf("==== read knote not readable?? %p", kn);
    }
}

int
evfilt_read_copyout(struct kevent *dst, struct knote *src, void *ptr)
{
    unsigned long bufsize;

    //struct event_buf * const ev = (struct event_buf *) ptr;

    /* TODO: handle regular files
       if (src->flags & KNFL_REGULAR_FILE) { ... } */

    memcpy(dst, &src->kev, sizeof(*dst));
    if (src->kn_flags & KNFL_PASSIVE_SOCKET) {
        /* TODO: should contains the length of the socket backlog */
        dst->data = 1;
    } else {
        /* On return, data contains the number of bytes of protocol
           data available to read.
         */
        if (ioctlsocket(src->kev.ident, FIONREAD, &bufsize) != 0) {
            dbg_wsalasterror("ioctlsocket");
            return (-1);
        }
        dst->data = bufsize;
    }

    return (0);
}

int
evfilt_write_copyout(struct kevent *dst, struct knote *src, void *ptr)
{
    //struct event_buf * const ev = (struct event_buf *) ptr;

    /* TODO: handle regular files
    if (src->flags & KNFL_REGULAR_FILE) { ... } */

    memcpy(dst, &src->kev, sizeof(*dst));
    dst->data = 1;

    return (0);
}
int
evfilt_rw_knote_create(struct filter *filt, struct knote *kn)
{
    struct knote *kn_mux = knote_lookup(filt->kf_rw_mux, kn->kev.ident);
    int rv = 0;
    if (kn_mux == NULL) {
        struct kevent mux_kev = kn->kev;
        mux_kev.flags = 0;
        mux_kev.fflags = filt->kf_rw_flag;
        rv = filter_knote_create(filt->kf_rw_mux, &kn_mux, &kn->kev);
        if (rv)
          return rv;
        assert(kn_mux != NULL);
    } else {
        struct kevent mux_kev = kn_mux->kev;
        mux_kev.flags = 0;
        mux_kev.fflags |= filt->kf_rw_flag;
        rv = filt->kf_rw_mux->kn_modify(filt->kf_rw_mux, kn_mux, &mux_kev);
        if (rv<0) {
            return rv;
        }
        knote_retain(kn_mux); // balace refcount wrt create path
    }
    kn->kn_mux = kn_mux;
    kn->kn_flags = kn->kn_mux->kn_flags; // copy over socket type
    knote_retain(kn->kn_mux); // we keep a long lived reference to mux knote
    if (filt->kf_rw_flag == NOTE_MUX_READ) {
        kn_mux->kn_mux_kn_read = kn;
        knote_retain(kn->kn_mux->kn_mux_kn_read); // mux keeps a long lived reference to read
    } else if(filt->kf_rw_flag == NOTE_MUX_WRITE) {
        kn_mux->kn_mux_kn_write = kn;
        knote_retain(kn->kn_mux->kn_mux_kn_write); // mux keeps a long lived reference to read
    } else {
        assert(0 && "MUX misuse");
    }
    knote_release(kn_mux); // release the reference we got from create or lookup
    return (rv);
}

int
evfilt_rw_knote_modify(struct filter *filt, struct knote *kn,
        const struct kevent *kev)
{
    return (-1); /* STUB */
}

int
evfilt_rw_knote_delete(struct filter *filt, struct knote *kn)
{
    assert(kn->kn_mux);
    if (filt->kf_rw_flag == NOTE_MUX_READ) {
        assert(kn == kn->kn_mux->kn_mux_kn_read);
        knote_release(kn->kn_mux->kn_mux_kn_read);
        kn->kn_mux->kn_mux_kn_read = NULL;
    } else if (filt->kf_rw_flag == NOTE_MUX_WRITE) {
        assert(kn == kn->kn_mux->kn_mux_kn_write);
        knote_release(kn->kn_mux->kn_mux_kn_write);
        kn->kn_mux->kn_mux_kn_write = NULL;
    } else {
        assert(0 && "MUX misuse");
    }
    if (kn->kn_mux->kn_mux_kn_read || kn->kn_mux->kn_mux_kn_write) {
        knote_release(kn->kn_mux);
    } else {
        knote_delete(filt->kf_rw_mux, kn->kn_mux);
    }
    kn->kn_mux = (void*)42;
    return (0);
}

int
evfilt_rw_knote_enable(struct filter *filt, struct knote *kn)
{
    struct kevent mux_kev = kn->kn_mux->kev;
    int rv = 0;
    mux_kev.flags = 0;
    mux_kev.fflags |= filt->kf_rw_flag;
    rv = filt->kf_rw_mux->kn_modify(filt->kf_rw_mux, kn->kn_mux, &mux_kev);
    return (rv);
}

int
evfilt_rw_knote_disable(struct filter *filt, struct knote *kn)
{
    struct kevent mux_kev = kn->kn_mux->kev;
    int rv = 0;
    mux_kev.flags = 0;
    mux_kev.fflags &= ~filt->kf_rw_flag;
    rv = filt->kf_rw_mux->kn_modify(filt->kf_rw_mux, kn->kn_mux, &mux_kev);
    return (rv);
}

int
evfilt_read_init(struct filter *filt)
{
    filt->kf_rw_mux = &filt->kf_kqueue->kq_rw_mux;
    filt->kf_rw_flag = NOTE_MUX_READ;
    filt->kf_rw_callback = evfilt_read_callback;
    return (0);
}

int
evfilt_write_init(struct filter *filt)
{
    filt->kf_rw_mux = &filt->kf_kqueue->kq_rw_mux;
    filt->kf_rw_flag = NOTE_MUX_WRITE;
    filt->kf_rw_callback = evfilt_write_callback;
    return (0);
}

const struct filter evfilt_read = {
    EVFILT_READ,
    evfilt_read_init,
    NULL,
    evfilt_read_copyout,
    evfilt_rw_knote_create,
    evfilt_rw_knote_modify,
    evfilt_rw_knote_delete,
    evfilt_rw_knote_enable,
    evfilt_rw_knote_disable,
};

const struct filter evfilt_write = {
    EVFILT_READ,
    evfilt_write_init,
    NULL,
    evfilt_write_copyout,
    evfilt_rw_knote_create,
    evfilt_rw_knote_modify,
    evfilt_rw_knote_delete,
    evfilt_rw_knote_enable,
    evfilt_rw_knote_disable,
};
