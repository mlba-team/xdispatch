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

cmake_minimum_required(VERSION 2.8)
include(CheckIncludeFiles)
include(CheckFunctionExists)
include(CheckSymbolExists)

project(kqueue)

# config.h
set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE=1)
check_symbol_exists( EPOLLRDHUP "sys/epoll.h" HAVE_DECL_EPOLLRDHUP)
check_symbol_exists( ppoll "poll.h" HAVE_DECL_PPOLL)
check_include_files( sys/epoll.h HAVE_SYS_EPOLL_H)
check_include_files( sys/event.h HAVE_SYS_EVENT_H)
check_include_files( sys/eventfd.h HAVE_SYS_EVENTFD_H)
check_include_files( sys/inotify.h HAVE_SYS_INOTIFY_H)
check_include_files( sys/signalfd.h HAVE_SYS_SIGNALFD_H)
check_include_files( sys/timerfd.h HAVE_SYS_TIMERFD_H)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)


# files
file(GLOB_RECURSE INCL include/*.h)
source_group(includes FILES ${INCL})

if(WIN32)
    file(GLOB SRC
        src/windows/*.h
        src/windows/*.c
        src/common/*.h
        src/*.c
        src/common/map.c
        src/common/filter.c
        src/common/knote.c
        src/common/kevent.c
        src/common/kqueue.c
    )
    add_definitions(
        -DLIBKQUEUE_EXPORTS
        -D_USRDLL
        -D_WINDLL
    )
    if(MINGW)
        SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -march=i486")
        SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=i486")
    endif()
else()
    file(GLOB SRC
        src/posix/*.h
        src/posix/platform.c
        src/linux/*.h
        src/linux/platform.c
        src/linux/signal.c
        src/linux/socket.c
        src/linux/timer.c
        src/linux/user.c
        src/linux/vnode.c
        src/linux/write.c
        src/linux/read.c
        src/common/*.h
        src/common/filter.c
        src/common/knote.c
        src/common/map.c
        src/common/kevent.c
        src/common/kqueue.c
        config.h.in
        config.h
    )
    include_directories(
        src/common
        include
    )
    mz_add_c_flag(GCC
        -Wall -Werror -Wextra -Wno-missing-field-initializers -g -O2
        -fpic
        -D_XOPEN_SOURCE=600
        -std=c99
        -fvisibility=hidden
    )
endif()
source_group(src FILES ${SRC})

#includes
include_directories(
    include
)

#library (static or shared)
option(STATIC_KQUEUE "Enable to build libkqueue as static lib" OFF)
if(STATIC_KQUEUE)
    message("-- building libkqueue as static lib")
    add_definitions(-DMAKE_STATIC)
    add_library(kqueue STATIC ${SRC} ${INCL})
else()
    add_library(kqueue SHARED ${SRC} ${INCL})
endif()

if(WIN32)
    set(LIB ${LIB} Ws2_32)
endif()
if(NOT WIN32)
    set(LIB ${LIB} pthread rt)
endif()
target_link_libraries(kqueue ${LIB})
set_target_properties(kqueue PROPERTIES DEBUG_POSTFIX "D")

#tests
option(KQUEUE_TESTS "Enable to build tests for libkqueue" OFF)
if(KQUEUE_TESTS)
    message("-- Adding tests for libkqueue")
    add_subdirectory(test)
endif()
