
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

# do we have native dispatch support on this platform?
CHECK_INCLUDE_FILES (off_dispatch/dispatch.h HAVE_NATIVE_DISPATCH_H)
if(HAVE_NATIVE_DISPATCH_H)
	message("   >> found native libDispatch implementation, using it")
endif()

# do we have kernel support for pthread_workqueues?
CHECK_INCLUDE_FILES (pthread_workqueue.h HAVE_PTHREAD_WORKQUEUE_H)
if(HAVE_PTHREAD_WORKQUEUE_H)
    message("   >> found native pthread_workqueue implementation, using it")
endif()

# do we have kqueues?
CHECK_INCLUDE_FILES (sys/event.h HAVE_KQUEUE_H)
if(HAVE_KQUEUE_H)
    message("   >> found native kqeue implementation, using it")
endif()
CHECK_INCLUDE_FILES (kqueue/sys/event.h HAVE_LIBKQUEUE_H)
if(HAVE_LIBKQUEUE_H)
    message("   >> found libkqeue installation, using it")
endif()

# add user space pthread_workqueue implementation (if needed)
if(NOT HAVE_PTHREAD_WORKQUEUE_H)
    mz_add_library(libpthread_workqueue libpthread_workqueue)
    INCLUDE_DIRECTORIES(
        libpthread_workqueue/include
    )
    set(LIBS ${LIBS} libpthread_workqueue)
endif()

# add user space kqueue implementation (if needed)
if(NOT HAVE_KQUEUE_H AND NOT HAVE_LIBKQUEUE_H)
    #mz_add_library(libkqueue libkqueue)
    INCLUDE_DIRECTORIES(
        libkqueue/include
    )
    #set(LIBS ${LIBS} libkqueue)
endif()

if(LINUX)
    set(LIBS ${LIBS} rt)
    if(HAVE_LIBKQUEUE_H)
        INCLUDE_DIRECTORIES(
            /usr/local/kqueue
        )
        set(LIBS ${LIBS} kqueue)
    endif()
endif()

if(NOT MZ_IS_VS)
    set(LIBS ${LIBS} pthread)
endif()

# core lib
file(GLOB INCL include/libdispatch/*.h)
file(GLOB CXX_INCL include/xdispatch/*.h)
file(GLOB CORE core/*.c core/*.h core/*.in)
file(GLOB SHIM core/shim/*.c core/shim/*.h)
file(GLOB CXX cxx/*.cpp cxx/*.h)
source_group(dispatch FILES ${INCL})
source_group(xdispatch FILES ${CXX_INCL})
source_group(core FILES ${CORE})
source_group(core/shim FILES ${SHIM})
source_group(cxx FILES ${CXX})


if(NOT HAVE_NATIVE_DISPATCH_H)
  
  # add new target
  add_library(${NAME} SHARED ${INCL} ${CORE} ${CXX_INCL} ${CXX} ${SHIM})
  
  # link this target with all needed libraries
  message("-- linking xdispatch with: ${LIBS}")
  target_link_libraries(${NAME} ${LIBS})
  mz_target_props(${NAME})
  
else()

	# detect compiler version
	_Boost_MZ_COMPILER_DUMPVERSION(GCC_VERSION)
	set(GCC_VERSION "${GCC_VERSION}")
	#message("${GCC_VERSION}")
	if(GCC_VERSION STREQUAL "45")
        mz_add_definition(HAVE_NATIVE_DISPATCH_H)
		mz_add_definition(HAVE_BLOCKS_IMPL)
		message("   >> bypassing Apple's crippled C++ blocks")
	endif()
	
	  # add new target
    add_library(${NAME} SHARED ${INCL} ${CXX} ${CXX_INCL})
  
      # link this target with all needed libraries
      if(MZ_IS_VS)
        target_link_libraries(${NAME} pthreads-w32)
      else()
        target_link_libraries(${NAME} pthread)
      endif()
      mz_target_props(${NAME})
	
endif()

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

# finally create the config.h
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/core/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/core/config.h)
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}
)
