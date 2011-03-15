##################################################
#
#	BUILD/GLOBAL.CMAKE
#
# 	This file is for providing a defined environment
#	of compiler definitions/macros and cmake functions
#	or variables throughout several projects. It can
#	be included twice or more without any issues.
#
#	(c) 2009-2010 Marius Zwicker
#
##################################################

### CONFIGURATION SECTION

# path to the mz tools files
if(NOT MZ_TOOLS_PATH)
	set(MZ_TOOLS_PATH "${BASE_DIR}/Build")
endif()

### END OF CONFIGURATION SECTION

# BOF: global.cmake
if(NOT HAS_MZ_GLOBAL)
	set(HAS_MZ_GLOBAL true)

# user info
message("-- configuring for build type: ${CMAKE_BUILD_TYPE}")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL Release)
	set(MAKE_RELEASE true)
else()
	set(MAKE_DEBUG true)
endif()
	
# detect compiler
include("${MZ_TOOLS_PATH}/compiler.cmake")

# hint to the code that 64bit is available
if(X64)
	message("-- Forcing 64bit")
	mz_add_definition("IS_64BIT")
endif()

# EOF: global.cmake
endif() 