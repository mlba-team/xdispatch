
cmake_minimum_required(VERSION 2.8)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckSymbolExists)

# start new subproject
set(XDISPATCH_VERSION 0.7.0devel)
project(xdispatch)

include(CTest)
enable_testing()

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
	endif()
endif()

# is the used compiler clang? if so we need blocks support
if(MZ_IS_CLANG AND NOT APPLE)
    message("-- linking with the BlocksRuntime")
    CHECK_INCLUDE_FILES (Block.h HAVE_BLOCK_H)
    if(NOT HAVE_BLOCK_H)
	mz_add_library(libblocksruntime libblocksruntime)
	include_directories( libblocksruntime )
    endif()
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
if(WIN32)
    install(FILES README.dox CHANGELOG.dox LICENSE NOTICE TODO.txt DESTINATION .)
endif()

# packaging information
if(MZ_64BIT)
    set(PACKAGE_ARCH "amd64")
else()
    set(PACKAGE_ARCH "i386")
endif()
if(MZ_IS_CLANG)
    set(PACKAGE_COMPILER "clang")
else()
    if(MZ_IS_VS)
        set(PACKAGE_COMPILER "msvc")
    else()
        set(PACKAGE_COMPILER "gcc")
    endif()
endif()

# package target
include(InstallRequiredSystemLibraries)
set(CPACK_COMPONENTS_ALL libxdispatch libQtDispatch)
set(CPACK_COMPONENT_LIBXDISPATCH_DISPLAY_NAME "XDispatch")
set(CPACK_COMPONENT_LIBXDISPATCH_DESCRIPTION "C++ API for Grand Central Dispatch")
set(CPACK_COMPONENT_LIBQTDISPATCH_DISPLAY_NAME "QtDispatch")
set(CPACK_COMPONENT_LIBQTDISPATCH_DESCRIPTION "Provides integration of Grand Central Dispatch into the Qt Event-Loop. Additionally a QThreadPool compatible API to Grand Central Dispatch is provided")
set(CPACK_COMPONENT_LIBQTDISPATCH_DEPENDS libxdispatch)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "userspace implementation of grand central dispatch")
set(CPACK_PACKAGE_VENDOR "www.mlba-team.de")
if(APPLE)
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.rtf")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.rtf")
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.rtf")
else()
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.dox")
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.dox")
endif()
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "7")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION "${XDISPATCH_VERSION}")

# package maker (OSX)
if(APPLE)
    set(CPACK_PACKAGING_INSTALL_PREFIX "/")
endif()
set(CPACK_OSX_PACKAGE_VERSION "10.6")

# deb package
set(CPACK_DEBIAN_PACKAGE_NAME "libxdispatch")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>=2.13-1) , libgcc1 (>=4.5.1), libstdc++6 (>=4.5.1)") #, libllvm2.9")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Marius Zwicker")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Userspace implementation of grand central dispatch")
set(CPACK_PACKAGE_DESCRIPTION "Provides an userspace implementation of the grand central dispatch api introduced with Mac OS X 10.6. Additionally a platform independent C++ interface is included")
set(CPACK_DEBIAN_PACKAGE_SECTION "Devel")
set(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}-${PACKAGE_COMPILER}-${PACKAGE_ARCH}")

# rpm package
set(CPACK_RPM_PACKAGE_NAME "libxdispatch")
set(CPACK_RPM_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}.${PACKAGE_COMPILER}.${PACKAGE_ARCH}")
set(CPACK_RPM_PACKAGE_LICENSE "Apache License, Version 2.0")
set(CPACK_RPM_PACKAGE_REQUIRES "glibc, libstdc++45")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "${PACAKGE_ARCH}")


SET(CPACK_PACKAGE_FILE_NAME "xdispatch_${CPACK_PACKAGE_VERSION}_${PACKAGE_COMPILER}_${PACKAGE_ARCH}")
include(CPack)
