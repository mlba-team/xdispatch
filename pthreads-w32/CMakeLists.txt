cmake_minimum_required(VERSION 2.8)

# start new subproject
set(NAME pthreads-w32)
project(${NAME})

# check if mz_tools is available
if(NOT HAS_MZ_GLOBAL)

        set(MZ_TOOLS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/global.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/macros.cmake)
        
endif()

# we only have this library on windows
if(WIN32 AND MZ_IS_VS)

# define file groups
set(INCL config.h implement.h semaphore.h pthread.h need_errno.h)

# add new exec target
add_library(${NAME} SHARED attr.c ${INCL}
	barrier.c
	cancel.c
	condvar.c
	exit.c
	misc.c
	mutex.c
	nonportable.c
	private.c
	rwlock.c
	sched.c
	semaphore.c
	spin.c
	sync.c
	tsd.c
	create.c
	dll.c
	errno.c
	fork.c
	global.c
	signal.c
	cleanup.c
	version.rc
)

# custom compiler flags
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Ob2 /W3 /MD /nologo /I. /D_WIN32_WINNT=0x400 /DHAVE_CONFIG_H  /DPTW32_BUILD /DPTW32_RC_MSC /D_CRT_SECURE_NO_WARNINGS /D__CLEANUP_C")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /Ob2 /W3 /MD /nologo /I. /D_WIN32_WINNT=0x400 /DHAVE_CONFIG_H  /DPTW32_BUILD /DPTW32_RC_MSC /D_CRT_SECURE_NO_WARNINGS /D__CLEANUP_C")


# link this target with all needed libraries
target_link_libraries(${NAME} wsock32 kernel32)
mz_target_props(${NAME})

endif()


