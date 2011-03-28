
cmake_minimum_required(VERSION 2.8)
INCLUDE (CheckIncludeFiles)

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
        set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -fvisibility=hidden)
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fvisibility=hidden)
	endif()
endif()

# do we have native dispatch support on this platform?
CHECK_INCLUDE_FILES (dispatch/dispatch.h HAVE_NATIVE_DISPATCH_H)
if(HAVE_NATIVE_DISPATCH_H)
	message("   >> found native libDispatch implementation, using it")
endif()

# do we have kernel support for pthread_workqueues?
CHECK_INCLUDE_FILES (pthread_workqueue.h HAVE_PTHREAD_WORKQUEUE_H)
if(HAVE_PTHREAD_WORKQUEUE_H)
    message("   >> found native pthread_workqueue implementation, using it")
endif()

# add pthreads on windows as well
mz_add_library(pthreads-w32 pthreads-w32)

# add user space pthread_workqueue implementation (if needed)
if(NOT HAVE_PTHREAD_WORKQUEUE_H)
    mz_add_library(libpthread_workqueue libpthread_workqueue)
    INCLUDE_DIRECTORIES(
        libpthread_workqueue/include
    )
    set(WORKQUEUE_LIB libpthread_workqueue)
endif()

# core lib
file(GLOB INCL include/libdispatch/*.h)
file(GLOB CXX_INCL include/xdispatch/*.h)
file(GLOB CORE core/*.c core/*.h)
file(GLOB CXX cxx/*.cpp cxx/*.h)
file(GLOB SHIM core/shim/*.h)
source_group(dispatch FILES ${INCL})
source_group(xdispatch FILES ${CXX_INCL})
source_group(core FILES ${CORE})
source_group(cxx FILES ${CXX})


if(NOT HAVE_NATIVE_DISPATCH_H)
  
  # add new target
  add_library(${NAME} SHARED ${INCL} ${CORE} ${CXX_INCL} ${CXX} ${SHIM})
  
  # link this target with all needed libraries
  if(MZ_IS_VS)
    target_link_libraries(${NAME} pthreads-w32 ${WORKQUEUE_LIB})
  else()
    target_link_libraries(${NAME} pthread ${WORKQUEUE_LIB})
  endif()
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
if (CMAKE_COMPILER_IS_GNU_CXX OR UNIX)
	# detect compiler version
	_Boost_MZ_COMPILER_DUMPVERSION(GCC_VERSION)
	set(GCC_VERSION "${GCC_VERSION}")
	if(GCC_VERSION STREQUAL "45")
		mz_add_definition(HAVE_BLOCKS_IMPL)
		message("   >> enabled tests for blocks")
	endif()
endif()
mz_add_executable(tests tests)

# documentation
include(docs/docgen.cmake)

