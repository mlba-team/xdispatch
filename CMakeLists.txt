
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
    find_package(Qt4 COMPONENTS QtGui QtMain QtCore QtNetwork QtTest)
	include(${QT_USE_FILE})
	
	# We need to ouput everything into the same directory
	if(WIN32)
		set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin CACHE PATH "Library output path")
		set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin CACHE PATH "Executable output path")
		message("-- Setting binary output path: ${CMAKE_BINARY_DIR}/Bin")
	else()
		set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE} CACHE PATH "Library output path")
		set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE} CACHE PATH "Executable output path")
		message("-- Setting binary output path: ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE}")
        mz_add_flag(-fvisibility=hidden)
	endif()
endif()

# does the user want to have clang with blocks support?
OPTION(CLANG_BLOCKS "Enable to use blocks with clang" OFF)
if(CLANG_BLOCKS)
    message("-- Linking with the BlocksRuntime")
    mz_add_flag(-fblocks)
    set(LIBS ${LIBS} BlocksRuntime)
endif()

CHECK_INCLUDE_FILES (sys/event.h FOUND_KQUEUE_IMPL)
CHECK_INCLUDE_FILES (kqueue/sys/event.h FOUND_LIBKQUEUE_IMPL)
CHECK_INCLUDE_FILES (pthread_workqueue.h FOUND_PTHREAD_WORKQUEUE_IMPL)
CHECK_INCLUDE_FILES (dispatch/dispatch.h HAVE_NATIVE_DISPATCH_H)

if(NOT FOUND_KQUEUE_IMPL AND NOT FOUND_LIBKQUEUE_IMPL AND NOT HAVE_NATIVE_DISPATCH_H)
    set(STATIC_KQUEUE On)
    mz_add_definition(MAKE_STATIC)
    mz_add_library(libkqueue libkqueue)
    include_directories(
    	libkqueue/include
    )
endif()

# currently there's a bug in mingw causing the
# builtin atomic opts not to work without this
if(MINGW)
	mz_add_flag( -march=i486)
endif()

# we hide all unneeded symbols
if(NOT WIN32)
	mz_add_flag(-fvisibility=hidden)
endif()

if(NOT FOUND_PTHREAD_WORKQUEUE_IMPL AND NOT HAVE_NATIVE_DISPATCH_H)
    set(STATIC_WORKQUEUE On)
    mz_add_definition(MAKE_STATIC)
    mz_add_library(libpthread_workqueue libpthread_workqueue)
    include_directories(
    	libpthread_workqueue/include
    )
endif()

# do we have native dispatch support on this platform?
if(HAVE_NATIVE_DISPATCH_H)
    message("   >> found native libDispatch implementation, using it")
else()
    mz_add_library(libdispatch core)
    set(LIBS ${LIBS} dispatch)
endif()

# the xdispatch target
mz_add_library(xdispatch cxx)

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
option(XDISPATCH_DOCS "Set to add a documentation ('Docs') target" OFF)
if(XDISPATCH_DOCS)
	include(docs/docgen.cmake)
endif()

# install target
install(DIRECTORY include DESTINATION . PATTERN "*.svn" EXCLUDE)
install(FILES README.dox CHANGELOG.dox LICENSE NOTICE TODO.txt DESTINATION .)

# package target
include(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "My funky project")
SET(CPACK_PACKAGE_VENDOR "MLBA Opensource")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.dox")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "5")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_FILE_NAME "xdispatch_${CPACK_PACKAGE_VERSION_MAJOR}_${CPACK_PACKAGE_VERSION_MINOR}_${CPACK_PACKAGE_VERSION_PATCH}__${CMAKE_SYSTEM_NAME}_${CMAKE_GENERATOR}")
include(CPack)
