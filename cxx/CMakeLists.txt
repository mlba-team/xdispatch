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
set(NAME xdispatch)
project(${NAME})

# core lib
file(GLOB CXX_INCL ../include/xdispatch/*.h ../include/xdispatch/dispatch)
file(GLOB CXX *.cpp *.h)
source_group(xdispatch FILES ${CXX_INCL})
source_group(cxx FILES ${CXX})
  
# res file
if(WIN32)
	set(RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
endif()
  
# add new target xdispatch
mz_add_definition(XDISPATCH_MAKEDLL)
if( IOS )
	set( XDISPATCH_BUILD_TYPE STATIC )
else()
	set( XDISPATCH_BUILD_TYPE SHARED )
endif()

add_library(${NAME} ${XDISPATCH_BUILD_TYPE} ${CXX_INCL} ${CXX} ${RES_FILE})
  
# link this target with all needed libraries
message("-- linking xdispatch with: ${LIBS}")
target_link_libraries(${NAME} ${LIBS})
mz_target_props(${NAME})

set_property(TARGET xdispatch PROPERTY VERSION ${XDISPATCH_VERSION})
set_property(TARGET xdispatch PROPERTY SOVERSION ${XDISPATCH_VERSION})

if(IOS)
    install(TARGETS xdispatch
                LIBRARY DESTINATION lib
                RUNTIME DESTINATION bin
                ARCHIVE DESTINATION lib)
    install(DIRECTORY ../include/xdispatch
                DESTINATION include
                PATTERN "*.svn" EXCLUDE)                
		
elseif(APPLE)
    set_property(TARGET xdispatch PROPERTY FRAMEWORK TRUE)
    set_property(TARGET xdispatch PROPERTY PUBLIC_HEADER ${CXX_INCL})

    install(TARGETS xdispatch
                COMPONENT libxdispatch
                FRAMEWORK DESTINATION "Library/Frameworks")
else()
    install(TARGETS xdispatch
                COMPONENT libxdispatch
                LIBRARY DESTINATION lib
                RUNTIME DESTINATION bin
                ARCHIVE DESTINATION lib)
    install(DIRECTORY ../include/xdispatch
                DESTINATION include
                COMPONENT libxdispatch-dev
                PATTERN "*.svn" EXCLUDE)
endif()

