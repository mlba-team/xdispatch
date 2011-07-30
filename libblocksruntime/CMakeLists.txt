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

SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DBlocksRuntime_EXPORTS -fPIC -std=c99 -Wall -Wextra -W -pedantic -Wno-unused-parameter")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} -DBlocksRuntime_EXPORTS -fPIC -std=c99 -Wall -Wextra -W -pedantic -Wno-unused-parameter")

OPTION(STATIC_BLOCKSRUNTIME "Enable to build libBlocksRuntime as static library" OFF)
if(STATIC_BLOCKSRUNTIME)
	add_library(BlocksRuntime STATIC ${HEADERS} ${SRC})
else()
	add_library(BlocksRuntime SHARED ${HEADERS} ${SRC})
endif()

set_target_properties(BlocksRuntime PROPERTIES DEBUG_POSTFIX "D")


