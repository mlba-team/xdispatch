##################################################
#
#	BUILD/COMPILER.CMAKE
#
# 	This file runs some tests for detecting
#	the compiler environment and provides a
#	crossplatform set of functions for setting
# 	compiler variables. If available features
#	for c++0x will be enabled automatically
#
#	(c) 2010 Marius Zwicker
#
##################################################

macro(mz_add_definition ...)
	if (MZ_IS_GCC)	
		SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D${ARGV0}" ${ARGV1})
		SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D${ARGV0}" ${ARGV1})
		SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -D${ARGV0}" ${ARGV1})
		SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D${ARGV0}" ${ARGV1})
	elseif(MZ_IS_VS)
		SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /D${ARGV0}" ${ARGV1})
		SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D${ARGV0}" ${ARGV1})
		SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /D${ARGV0}" ${ARGV1})
		SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /D${ARGV0}" ${ARGV1})
	endif()
endmacro()

	# coming from find_boost
	#
	# Runs compiler with "-dumpversion" and parses major/minor
	# version with a regex.
	#
	FUNCTION(_Boost_MZ_COMPILER_DUMPVERSION _OUTPUT_VERSION)

	  EXEC_PROGRAM(${CMAKE_CXX_COMPILER}
		ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion
		OUTPUT_VARIABLE _boost_COMPILER_VERSION
	  )
	  STRING(REGEX REPLACE "([0-9])\\.([0-9])(\\.[0-9])?" "\\1\\2"
		_boost_COMPILER_VERSION ${_boost_COMPILER_VERSION})

	  SET(${_OUTPUT_VERSION} ${_boost_COMPILER_VERSION} PARENT_SCOPE)
	ENDFUNCTION()

# we only run the first time
if(NOT MZ_COMPILER_TEST_HAS_RUN)

	message("-- running mz compiler detection tools")
	
	# compiler settings and defines depending on platform
	if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
		set(DARWIN TRUE CACHE BOOL DARWIN  )
	elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
		set(LINUX TRUE CACHE BOOL LINUX )
	else()
		set(WINDOWS TRUE CACHE BOOL WINDOWS )
	endif()
	
	# gnu compiler
	#message("IS_GCC ${CMAKE_COMPILER_IS_GNU_CC}")
	if(UNIX OR MINGW)
		message("-- Gnu GCC compiler found")
		
		set(MZ_IS_GCC TRUE CACHE BOOL MZ_IS_GCC)
	
		# detect compiler version
		_Boost_MZ_COMPILER_DUMPVERSION(GCC_VERSION)
		set(GCC_VERSION "${GCC_VERSION}")
		if(GCC_VERSION STREQUAL "45")
			message("-- C++0x support detected")
			set(MZ_HAS_CXX0X TRUE CACHE BOOL MZ_HAS_CXX0X)
		endif()
		
		set(MZ_COMPILER_TEST_HAS_RUN TRUE CACHE BOOL MZ_COMPILER_TEST_HAS_RUN)
	
	# ms visual studio
	elseif(MSVC OR MSVC_IDE)
		message("-- Microsoft Visual Studio Compiler found")
		
		set(MZ_IS_VS TRUE CACHE BOOL MZ_IS_VS)
		
		if(MSVC10)
			message("-- C++0x support detected")
			set(MZ_HAS_CXX0X TRUE CACHE BOOL MZ_HAS_CXX0X )
		endif()

		set(MZ_COMPILER_TEST_HAS_RUN TRUE CACHE BOOL MZ_COMPILER_TEST_HAS_RUN)
		
	# currently unsupported
	else()
		message(FATAL_ERROR "!! compiler platform currently unsupported by mz tools !!")
	endif()

endif() #MZ_COMPILER_TEST_HAS_RUN

# compiler flags
if(MZ_IS_GCC)
		# default macros and configuration
		if(WINDOWS) # windows would be defined otherwise, this collides with some qt headers
			SET(CMAKE_C_FLAGS_DEBUG "-ggdb -DDEBUG ${TARGET_DEFS} -Wall -Werror -Wno-unused-function -D${CMAKE_SYSTEM_PROCESSOR} -DWIN32")
			SET(CMAKE_C_FLAGS_RELEASE "-D${CMAKE_SYSTEM_PROCESSOR} -Wall -Werror -Wno-unused-function ${TARGET_DEFS} -DWIN32 -O3")
			SET(CMAKE_CXX_FLAGS_DEBUG "-ggdb -DDEBUG -D${CMAKE_SYSTEM_PROCESSOR} ${TARGET_DEFS} -Wall -Werror -Wno-unused-function -DWIN32")
			SET(CMAKE_CXX_FLAGS_RELEASE "-D${CMAKE_SYSTEM_PROCESSOR} -Wall -Werror -Wno-unused-function ${TARGET_DEFS} -DWIN32 -O3")
		else()
			SET(CMAKE_C_FLAGS_DEBUG "-ggdb -DDEBUG ${TARGET_DEFS} -Wall -Werror -Wno-unused-function -D${CMAKE_SYSTEM_PROCESSOR} -D${CMAKE_SYSTEM_NAME}")
			SET(CMAKE_C_FLAGS_RELEASE "-D${CMAKE_SYSTEM_PROCESSOR} -Wall -Werror -Wno-unused-function ${TARGET_DEFS} -D${CMAKE_SYSTEM_NAME} -O3")
			SET(CMAKE_CXX_FLAGS_DEBUG "-ggdb -DDEBUG -D${CMAKE_SYSTEM_PROCESSOR} ${TARGET_DEFS} -Wall -Werror -Wno-unused-function -D${CMAKE_SYSTEM_NAME}")
			SET(CMAKE_CXX_FLAGS_RELEASE "-D${CMAKE_SYSTEM_PROCESSOR} -Wall -Werror -Wno-unused-function ${TARGET_DEFS} -D${CMAKE_SYSTEM_NAME} -O3")
		endif()
		
		if(WINDOWS)
			mz_add_definition("WIN32_MINGW")
		endif()
		
		# optional C++0x features on gcc (on vs2010 this is enabled by default)
		if(MZ_HAS_CXX0X) # AND NOT DARWIN)
			SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=gnu++0x")
			SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=gnu++0x")
			message("-- forcing C++0x support on this platform")
		endif()

elseif(MZ_IS_VS)
		# default macros and configuration
		SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MP /D DEBUG /D WIN32 /D WIN32_VS ${TARGET_DEFS}")
		SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MP /D WIN32 /D WIN32_VS  /O2 ${TARGET_DEFS}")
		SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MP /D DEBUG /D WIN32 /D WIN32_VS  ${TARGET_DEFS}")
		SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MP /D WIN32 /D WIN32_VS  /O2 ${TARGET_DEFS}")
endif()