
cmake_minimum_required(VERSION 2.8)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckSymbolExists)

# start new subproject
project(xdispatch)
include(package/version.cmake)

# ctest config
option(BUILD_XDISPATCH_TESTS "Enable to build the xdispatch test suite" OFF)
include(CTest)
enable_testing()

# check if mz_tools is available
if(NOT HAS_MZ_GLOBAL)

	set(MZ_TOOLS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build")
	include(build/global.cmake)

	# QT settings
  find_package(Qt4 COMPONENTS QtGui QtMain QtCore QtNetwork QtTest)
	include(${QT_USE_FILE})
	
	# We need to ouput everything into the same directory
  set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/ CACHE PATH "Library output path")
  set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/ CACHE PATH "Executable output path")
  message("-- Setting binary output path: ${CMAKE_BINARY_DIR}/bin/")
endif()

# is the used compiler clang? if so we need blocks support
if(MZ_IS_CLANG AND NOT APPLE)
    message("-- linking with the BlocksRuntime")
    mz_find_include_library(BlocksRuntime 
        SYS 0 
        SRC libblocksruntime libblocksruntime BlocksRuntime
    )

    mz_add_flag(CLANG -fblocks)
    include_directories(${BLOCKSRUNTIME_INCLUDE_DIRS})
    set(LIBS ${LIBS} ${BLOCKSRUNTIME_LIBRARIES})
endif()

# are we running on mac os and want to build universal binaries?
if(APPLE)
	option(XDISPATCH_UNIVERSAL "Enable to build universal binaries" off)
	if( XDISPATCH_UNIVERSAL )
		message("-- building universal binaries")
		mz_add_flag(GCC -arch i386 -arch x86_64)
	endif()
endif()

check_include_files(sys/event.h FOUND_KQUEUE_IMPL)
check_include_files(kqueue/sys/event.h FOUND_LIBKQUEUE_IMPL)
if(FOUND_KQUEUE_IMPL OR FOUND_LIBKQUEUE_IMPL)
    option(FORCE_OWN_LIBKQUEUE "Enable to force the use of the shipped libkqueue version" OFF)
else()
    option(FORCE_OWN_LIBKQUEUE "Enable to force the use of the shipped libkqueue version" ON)
endif()

check_include_files(pthread_workqueue.h FOUND_PTHREAD_WORKQUEUE_IMPL)
if(FOUND_PTHREAD_WORKQUEUE_IMPL)
    option(FORCE_OWN_LIBPTHREAD_WORKQUEUE "Enable to force the use of the shipped libpthread_workqueue version" OFF)
else()
    option(FORCE_OWN_LIBPTHREAD_WORKQUEUE "Enable to force the use of the shipped libpthread_workqueue version" ON)
endif()

check_include_files(dispatch/dispatch.h HAVE_NATIVE_DISPATCH_H)
if(HAVE_NATIVE_DISPATCH_H)
    option(FORCE_OWN_LIBDISPATCH "Enable to force the use of the shipped libdispatch version" OFF)
else()
    option(FORCE_OWN_LIBDISPATCH "Enable to force the use of the shipped libdispatch version" ON)
endif()

if(FORCE_OWN_LIBKQUEUE AND FORCE_OWN_LIBDISPATCH)
    option(STATIC_KQUEUE "" ON)
    mz_add_definition(MAKE_STATIC=1 STATIC_KQUEUE=1)
    mz_add_library(libkqueue libkqueue)
    include_directories(
    	libkqueue/include
    )
endif()

# currently there's a bug in mingw causing the
# builtin atomic opts not to work without this
if(MINGW AND MZ_32BIT)
	mz_add_flag(GCC -march=i486)
endif()

# we hide all unneeded symbols
if(NOT WIN32)
	mz_add_flag(GCC -fvisibility=hidden)
endif()

if(FORCE_OWN_LIBPTHREAD_WORKQUEUE AND FORCE_OWN_LIBDISPATCH)
    option(STATIC_WORKQUEUE "" ON)
    mz_add_definition(MAKE_STATIC=1 STATIC_WORKQUEUE=1)
    mz_add_library(libpthread_workqueue libpthread_workqueue)
    include_directories(
    	libpthread_workqueue/include
    )
endif()

# do we have native dispatch support on this platform?
if(NOT FORCE_OWN_LIBDISPATCH)
    message("   >> using system installed libdispatch")
else()
    mz_add_library(libdispatch core)
    set(LIBS ${LIBS} dispatch)
endif()

# the xdispatch target
mz_add_library(xdispatch cxx)


# Qt Extension if Qt is available
if(QT4_FOUND)
	option(XDISPATCH_QT_SUPPORT "Enable to build QtDispatch" ON)
else()
	option(XDISPATCH_QT_SUPPORT "Enable to build QtDispatch" OFF)
endif()

if(XDISPATCH_QT_SUPPORT)
	mz_add_library(QtDispatch Qt)
endif()

# copy the includes to a specific folder
# to mimic a proper installation. This is
# due to us needing different headers on different
# platforms (especially Darwin)
if(APPLE OR NOT FORCE_OWN_LIBDISPATCH)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/xdispatch DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include)
    if(XDISPATCH_QT_SUPPORT)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/QtDispatch DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include)
    endif()
    if(FORCE_OWN_LIBDISPATCH)
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/dispatch DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include)
    endif()
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
else()
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

# custom test framework for xdispatch
if(BUILD_XDISPATCH_TESTS)
	mz_add_executable(tests tests)
endif()

# documentation
option(XDISPATCH_DOCS "Set to add a documentation ('Docs') target" OFF)
if(XDISPATCH_DOCS)
	include(docs/docgen.cmake)
endif()

# packaging

if(APPLE)
	include(package/osx/pkg.cmake)
elseif(UNIX)
	include(package/rpm/rpm.cmake)
	include(package/debian/deb.cmake)	
    include(package/ubuntu/deb.cmake)
else() # Windows ...
	include(package/windows/zip.cmake)
endif()
	

include(CPack)
