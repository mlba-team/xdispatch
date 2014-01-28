#
# Copyright (c) 2011-2013 MLBA-Team. All rights reserved.
#
# @MLBA_OPEN_LICENSE_HEADER_START@
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# @MLBA_OPEN_LICENSE_HEADER_END@
#

cmake_minimum_required(VERSION 2.8)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckFunctionExists)
INCLUDE (CheckSymbolExists)

# start new subproject
set(NAME dispatch)
project(${NAME} C)

# do we have kernel support for pthread_workqueues?
if(NOT APPLE)
    set(LIBS ${LIBS} pthread_workqueue)
endif()

# do we have kqueue?
if(FORCE_OWN_LIBKQUEUE)
    set(LIBS ${LIBS} kqueue)
endif()

# other needed functions
check_include_files( mach/mach_time.h HAVE_MACH_ABSOLUTE_TIME)
check_include_files( stdint.h HAVE_STDINT_H )
check_include_files( crtdbg.h HAVE_CRTDBG_H )
check_include_files( pthread.h HAVE_PTHREAD_H )
check_function_exists( getprogname HAVE_GETPROGNAME)
check_function_exists( getexecname HAVE_GETEXECNAME)
check_function_exists( program_invocation_short_name HAVE_DECL_PROGRAM_INVOCATION_SHORT_NAME)
check_symbol_exists( CLOCK_UPTIME "time.h" HAVE_DECL_CLOCK_UPTIME)
check_symbol_exists( CLOCK_MONOTONIC "time.h" HAVE_DECL_CLOCK_MONOTONIC)
check_function_exists( sysconf HAVE_SYSCONF)

if(LINUX)
    set(LIBS ${LIBS} rt m)
endif()

if(NOT WIN32)
    set(LIBS ${LIBS} pthread)
endif()

if(MINGW)
    set(LIBS ${LIBS} ws2_32)
endif()

# platforms
if(WIN32)
	file(GLOB PLATFORM_OS platform/windows/*.c platform/windows/*.h)
	source_group(platform\\windows FILES ${PLATFORM_OS})
elseif(APPLE)
	file(GLOB PLATFORM_OS platform/darwin/*.c platform/darwin/*.h)
	source_group(platform\\darwin FILES ${PLATFORM_OS})
else()
	file(GLOB PLATFORM_OS platform/posix/*.c platform/posix/*.h)
	source_group(platform\\posix FILES ${PLATFORM_OS})
endif()
file(GLOB PLATFORM platform/*.h platform/*.c)
source_group(platform FILES ${PLATFORM})

# core lib
file(GLOB INCL include/dispatch/*.h)
file(GLOB CORE src/*.c src/*.h config/*.in)
file(GLOB SHIM src/shims/*.c src/shims/*.h)
source_group(dispatch FILES ${INCL})
source_group(core FILES ${CORE})
source_group(core\\shims FILES ${SHIM})

# res file
if(WIN32)
	set(RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
endif()
  
# add new target
add_definitions(-DDISPATCH_MAKEDLL)
add_definitions(-D_GNU_SOURCE)
if(XDISPATCH_QT_SUPPORT)
    add_definitions(-DXDISPATCH_QT_SUPPORT)
endif()
add_library(${NAME} SHARED ${INCL} ${CORE} ${SHIM} ${PLATFORM_OS} ${PLATFORM} ${RES_FILE})
  
# link this target with all needed libraries
message("-- linking dispatch with: ${LIBS}")
target_link_libraries(${NAME} ${LIBS})

set_target_properties( dispatch PROPERTIES
    DEBUG_POSTFIX "D"
    VERSION ${XDISPATCH_VERSION}
    SOVERSION ${XDISPATCH_VERSION}
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"
)

# finally create the config.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config/config.h)
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)


if(APPLE)
    set_property(TARGET dispatch PROPERTY FRAMEWORK TRUE)
    set_property(TARGET dispatch PROPERTY PUBLIC_HEADER ${INCL})

    install(TARGETS dispatch
                COMPONENT libdispatch
                FRAMEWORK DESTINATION "Library/Frameworks")
else()
    install(TARGETS dispatch
                COMPONENT libdispatch
                LIBRARY DESTINATION lib
                RUNTIME DESTINATION bin
                ARCHIVE DESTINATION lib)
    install(DIRECTORY ../include/dispatch
                DESTINATION include
                COMPONENT libdispatch-dev
                PATTERN "*.svn" EXCLUDE)
endif()
