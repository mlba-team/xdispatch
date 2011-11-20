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
set(MZ_TOOLS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Build")

### END OF CONFIGURATION SECTION

# BOF: global.cmake
if(NOT HAS_MZ_GLOBAL)
	set(HAS_MZ_GLOBAL true)

  # detect compiler
  include("${MZ_TOOLS_PATH}/compiler.cmake")

  # user info
  message("-- configuring for build type: ${CMAKE_BUILD_TYPE}")

# EOF: global.cmake
endif() 