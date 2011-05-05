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

cmake_minimum_required(VERSION 2.8)
INCLUDE (CheckIncludeFiles)

project(pthread_workqueue)

#files
file(GLOB_RECURSE INCL include/*.h)
source_group(includes FILES ${INCL})

if(WIN32)
	file(GLOB SRC 
		src/windows/*.h
		src/windows/*.c
		src/*.h
		src/*.c
	)
	add_definitions(
		-DLIBPTHREAD_WORKQUEUE_EXPORTS
		-D_USRDLL
		-D_WINDLL
	)
else()
	file(GLOB SRC 
		src/posix/*.h
		src/posix/*.c
		src/*.h
		src/*.c
	)
        include_directories(
            src
        )
        add_definitions(
            -DHAVE_ERR_H=1
            -D_XOPEN_SOURCE=600
            -D_GNU_SOURCE
            -std=c99
            -fPIC
        )
endif()
source_group(src FILES ${SRC})

#includes
include_directories(
	include
)

#library (static or shared)
option(STATIC_WORKQUEUE "Enable to build libpthread_workqueue as static lib" OFF)
if(STATIC_WORKQUEUE)
    add_definitions(-DMAKE_STATIC)
    add_library(pthread_workqueue STATIC ${SRC} ${INCL})
else()
    add_library(pthread_workqueue SHARED ${SRC} ${INCL})
endif()
if(NOT WIN32)
	target_link_libraries(pthread_workqueue pthread)
endif()
set_target_properties(pthread_workqueue PROPERTIES DEBUG_POSTFIX "D")

#tests
option(WORKQUEUE_TESTS "Enable to build tests for libpthread_workqueue" OFF)
if(WORKQUEUE_TESTS)
        message("-- Adding tests for libpthread_workqueue")
	add_subdirectory(testing)
endif()

