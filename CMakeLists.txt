
cmake_minimum_required(VERSION 2.8)
INCLUDE (CheckIncludeFiles)

# start new subproject
set(NAME xdispatch)
project(${NAME})

# check if mz_tools is available
if(NOT HAS_MZ_GLOBAL)

	set(MZ_TOOLS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/global.cmake)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/macros.cmake)
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/qtmacros.cmake)

	# QT settings
	set(QT_USE_OPENGL TRUE)
	include(${QT_USE_FILE})

	# include qt headers (if found)
	INCLUDE_DIRECTORIES(
		${QT_QTCORE_INCLUDE_DIR}
		${QT_QTTEST_INCLUDE_DIR}
	)
	
	# We need to ouput everything into the same directory
	if(MZ_IS_VS)
		set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin CACHE PATH "Library output path")
		set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin CACHE PATH "Executable output path")
		message("-- Setting binary output path: ${CMAKE_BINARY_DIR}/Bin")
	else()
		set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE} CACHE PATH "Library output path")
		set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE} CACHE PATH "Executable output path")
		message("-- Setting binary output path: ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE}")
	endif()
endif()

# add pthreads on windows as well
mz_add_library(pthreads-w32 pthreads-w32)

# do we have native dispatch support on this platform?
CHECK_INCLUDE_FILES (dispatch/dispatch.h HAVE_NATIVE_DISPATCH_H)
if(HAVE_NATIVE_DISPATCH_H)
	message("   >> found native libDispatch implementation, using it")
endif()

# do we have stdint?
CHECK_INCLUDE_FILES (stdint.h HAVE_STDINT_H)
if(NOT HAVE_STDINT_H)
	if(MZ_IS_VS)
		INCLUDE_DIRECTORIES(
			${CMAKE_CURRENT_SOURCE_DIR}/win
		)
	else()
		fail("stdint.h needed but not found")
	endif()
endif()

# blocks extension
set(BC bc/blocks.h)
source_group(blocks FILES ${BC})

# core lib
file(GLOB INCL *.h)
if(NOT HAVE_NATIVE_DISPATCH_H)
	# custom implementation
	file(GLOB CORE core/*.c core/*.cpp core/*.h)
# hack as C++ is currently broken on OS X 10.6 (gcc 4.2) (gcc 4.5.1 only)
else()
	# detect compiler version
	_Boost_MZ_COMPILER_DUMPVERSION(GCC_VERSION)
	set(GCC_VERSION "${GCC_VERSION}")
	#message("${GCC_VERSION}")
	if(GCC_VERSION STREQUAL "45")
		file(GLOB CORE core/*.cpp core/runBlocks.h core/queue_internal.h)
		# add_definitions is borked in cmake-2.8
		mz_add_definition(HAVE_NATIVE_DISPATCH_H)
		mz_add_definition(HAVE_BLOCKS_IMPL)
		message("   >> bypassing Apple's crippled C++ blocks")
	endif()
endif()
source_group(includes FILES ${INCL})
source_group(core FILES ${CORE})

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
	file(GLOB QTEXT Qt/*.h Qt/*.cpp)
	file(GLOB QHEADER Qt/*.h)

	# moc qt files
	qt4_wrap_cpp(MOC ${QHEADER})

	source_group(QtSupport FILES ${QTEXT})
endif()


# add new exec target
add_library(${NAME} STATIC ${INCL} ${BC} ${CORE} ${QTEXT} ${MOC})

# link this target with all needed libraries
if(MZ_IS_VS)
	target_link_libraries(${NAME} pthreads-w32 ${QT_LIBRARIES})
else()
	target_link_libraries(${NAME} pthread ${QT_LIBRARIES})
endif()
mz_target_props(${NAME})

# thanks to c++0x we do not need a blocks compiler
#include(bc/CMakeLists.txt)

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
mz_add_target(tests tests)
