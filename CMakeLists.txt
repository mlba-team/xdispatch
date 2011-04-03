
cmake_minimum_required(VERSION 2.8)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckSymbolExists)

# start new subproject
set(NAME xdispatch)
project(${NAME})

# check if mz_tools is available
if(NOT HAS_MZ_GLOBAL)

	set(MZ_TOOLS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Build")
	include(${CMAKE_CURRENT_SOURCE_DIR}/Build/global.cmake)
	include(${CMAKE_CURRENT_SOURCE_DIR}/Build/macros.cmake)

	# QT settings
    find_package(Qt4 COMPONENTS QtGui QtMain QtCore QtTest)
	include(${QT_USE_FILE})
	
	# We need to ouput everything into the same directory
	if(MZ_IS_VS)
		set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin CACHE PATH "Library output path")
		set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin CACHE PATH "Executable output path")
		message("-- Setting binary output path: ${CMAKE_BINARY_DIR}/Bin")
	else()
		set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE} CACHE PATH "Library output path")
		set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE} CACHE PATH "Executable output path")
		message("-- Setting binary output path: ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE}")
        mz_add_flag(-fvisibility=hidden)
        mz_add_flag(-fvisibility=hidden)
	endif()
endif()

CHECK_INCLUDE_FILES (sys/event.h FOUND_KQUEUE_IMPL)
CHECK_INCLUDE_FILES (kqueue/sys/event.h FOUND_LIBKQUEUE_IMPL)
if(NOT FOUND_KQUEUE_IMPL AND NOT FOUND_LIBKQUEUE_IMPL)
    mz_add_library(libkqueue libkqueue)
    include_directories(
    	libkqueue/include
    )
endif()
CHECK_INCLUDE_FILES ("pthread_workqueue.h" FOUND_PTHREAD_WORKQUEUE_IMPL)
if(NOT FOUND_PTHREAD_WORKQUEUE_IMPL)
    mz_add_library(libpthread_workqueue libpthread_workqueue)
    include_directories(
    	libpthread_workqueue/include
    )
endif()

# do we have native dispatch support on this platform?
CHECK_INCLUDE_FILES (off_dispatch/dispatch.h HAVE_NATIVE_DISPATCH_H)
if(HAVE_NATIVE_DISPATCH_H)
	message("   >> found native libDispatch implementation, using it")
else()
	mz_add_library(libdispatch core)
	set(LIBS ${LIBS} dispatch)
endif()

# core lib
file(GLOB CXX_INCL include/xdispatch/*.h)
file(GLOB CXX cxx/*.cpp cxx/*.h)
source_group(xdispatch FILES ${CXX_INCL})
source_group(cxx FILES ${CXX})
  
# add new target
add_library(${NAME} SHARED ${CXX_INCL} ${CXX})
  
# link this target with all needed libraries
message("-- linking xdispatch with: ${LIBS}")
target_link_libraries(${NAME} ${LIBS})
mz_target_props(${NAME})

# tell everybody we have libdispatch support here, perhaps even native by the OS
if(HAVE_NATIVE_DISPATCH_H)
	if(PARENT_SCOPE)
        mz_add_definition("HAVE_NATIVE_DISPATCH_H" PARENT_SCOPE)
	else()
        mz_add_definition("HAVE_NATIVE_DISPATCH_H")
	endif()
endif()

# Use HAS_LIBDISPATCH to query it in other projects
if(PARENT_SCOPE)
	mz_add_definition("HAS_LIBDISPATCH" PARENT_SCOPE)
	set(HAS_LIBDISPATCH TRUE PARENT_SCOPE)
endif()

# Qt Extension if Qt is available
if(QT4_FOUND)
	mz_add_library(QtDispatch Qt)
endif()

# custom test framework for xdispatch
mz_add_executable(tests tests)

# documentation
include(docs/docgen.cmake)
