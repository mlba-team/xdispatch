#
# Copyright (c) 2011 Marius Zwicker <marius@mlba-team.de>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

mz_use_default_compiler_settings()

#files
if(UNIX)
    set(SRC
        main.c
        kevent.c
        test.c
        proc.c
        read.c
        signal.c
        timer.c
        vnode.c
        user.c
    )
    mz_add_c_flag(GCC
        -g -O0 -Wall -Werror
    )
else()
    set(SRC
        common.h
        kevent.c
        main.c
        read.c
        test.c
        timer.c
        user.c
        vnode.c
    )
endif()


#includes
include_directories(
	../include
)

if(UNIX)
    set(LIBS pthread rt)
else()
    set(LIBS wsock32)
endif()

add_executable(kqueue-test ${SRC})
target_link_libraries(kqueue-test kqueue ${LIBS})
set_target_properties(kqueue-test PROPERTIES DEBUG_POSTFIX "D")
