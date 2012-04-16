
cmake_minimum_required(VERSION 2.8)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckFunctionExists)
INCLUDE (CheckSymbolExists)

# start new subproject
set(NAME dispatch)
project(${NAME} C)

# do we have kernel support for pthread_workqueues?
CHECK_INCLUDE_FILES (pthread_workqueue.h HAVE_PTHREAD_WORKQUEUE_H)
if(HAVE_PTHREAD_WORKQUEUE_H)
    if(LINUX)
        message("   >> found libpthread_workqueue installation, using it")
        set(LIBS ${LIBS} pthread_workqueue)
    else()
        message("   >> found native pthread_workqueue implementation, using it")
    endif()
else()
    set(LIBS ${LIBS} pthread_workqueue)
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
if(NOT HAVE_KQUEUE_H)
    set(LIBS ${LIBS} kqueue)
endif()

# other needed functions
CHECK_INCLUDE_FILES( mach/mach_time.h HAVE_MACH_ABSOLUTE_TIME)
CHECK_INCLUDE_FILES( stdint.h HAVE_STDINT_H )
CHECK_INCLUDE_FILES( crtdbg.h HAVE_CRTDBG_H )
CHECK_INCLUDE_FILES( pthread.h HAVE_PTHREAD_H )
CHECK_FUNCTION_EXISTS( getprogname HAVE_GETPROGNAME)
CHECK_FUNCTION_EXISTS( getexecname HAVE_GETEXECNAME)
CHECK_FUNCTION_EXISTS( program_invocation_short_name HAVE_DECL_PROGRAM_INVOCATION_SHORT_NAME)
CHECK_SYMBOL_EXISTS( CLOCK_UPTIME "time.h" HAVE_DECL_CLOCK_UPTIME)
CHECK_SYMBOL_EXISTS( CLOCK_MONOTONIC "time.h" HAVE_DECL_CLOCK_MONOTONIC)
CHECK_FUNCTION_EXISTS( sysconf HAVE_SYSCONF)

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
file(GLOB INCL ../include/dispatch/*.h)
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
add_library(${NAME} SHARED ${INCL} ${CORE} ${SHIM} ${PLATFORM_OS} ${PLATFORM} ${RES_FILE})
  
# link this target with all needed libraries
message("-- linking dispatch with: ${LIBS}")
target_link_libraries(${NAME} ${LIBS})
set_target_properties(${NAME} PROPERTIES DEBUG_POSTFIX "D")

set_property(TARGET dispatch PROPERTY VERSION ${XDISPATCH_VERSION})
set_property(TARGET dispatch PROPERTY SOVERSION ${XDISPATCH_VERSION})

# finally create the config.h
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config/config.h)
INCLUDE_DIRECTORIES(
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
