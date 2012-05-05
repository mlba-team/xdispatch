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

project(BlocksRuntime)

set(HEADERS Block.h Block_private.h)
set(SRC runtime.c data.c)

mz_add_flag(CLANG -DBlocksRuntime_EXPORTS -fPIC -std=c99 -Wall -Wextra -W -pedantic -Wno-unused-parameter)

OPTION(STATIC_BLOCKSRUNTIME "Enable to build libBlocksRuntime as static library" OFF)
if(STATIC_BLOCKSRUNTIME)
	add_library(BlocksRuntime STATIC ${HEADERS} ${SRC})
else()
	add_library(BlocksRuntime SHARED ${HEADERS} ${SRC})
endif()

set_target_properties(BlocksRuntime PROPERTIES DEBUG_POSTFIX "D")
set_property(TARGET BlocksRuntime PROPERTY VERSION 0.1-1)
set_property(TARGET BlocksRuntime PROPERTY SOVERSION 0.1-1)

            
if(APPLE)
    set_property(TARGET BlocksRuntime PROPERTY FRAMEWORK TRUE)
    set_property(TARGET BlocksRuntime PROPERTY PUBLIC_HEADER Block.h)

    install(TARGETS BlocksRuntime
                COMPONENT libxdispatch
                FRAMEWORK DESTINATION "Library/Frameworks")
else()
    install(TARGETS BlocksRuntime
                COMPONENT libxdispatch
                LIBRARY DESTINATION lib
                RUNTIME DESTINATION bin
                ARCHIVE DESTINATION lib)
    install(FILES Block.h Block_private.h
            DESTINATION include
            COMPONENT libxdispatch)
endif()            
