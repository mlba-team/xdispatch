
##
## Used to switch between devel and release
##
#set( IS_A_RELEASE ON )

##
## Create all version info
##

# obtain the version info using subversion
find_package(Subversion)
if(SUBVERSION_FOUND)
    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} XDISPATCH)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/package/revision.in ${CMAKE_CURRENT_SOURCE_DIR}/package/revision)
elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/package/revision) # use (hopefully) cached version
    file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/package/revision XDISPATCH_WC_REVISION LIMIT_COUNT 1)
else()
	set(XDISPATCH_WC_REVISION "")
endif()

# Variables referring to the system the packaging took place at
if(XDISPATCH_UNIVERSAL)
	set( PACKAGE_ARCH			"universal") 
elseif(MZ_64BIT)
    set( PACKAGE_ARCH 			"amd64")
else()
    set( PACKAGE_ARCH 			"i386")
endif()
if(MZ_IS_CLANG)
    set( PACKAGE_COMPILER 		"clang")
elseif(MZ_IS_VS)
	set( PACKAGE_COMPILER           "msvc")
else()
	set( PACKAGE_COMPILER           "gcc")
endif()

# Variables describing the version and functionality of 
# libxdispatch will be updated on each new version release

set( XDISPATCH_VERSION_MAJOR            0)
set( XDISPATCH_VERSION_MINOR            8)
set( XDISPATCH_VERSION_PATCH            0)
if( NOT IS_A_RELEASE )
  if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set( XDISPATCH_VERSION_SUFFIX       devel${XDISPATCH_WC_REVISION})
    message("-- Configuring release version")
  else()
    set( XDISPATCH_VERSION_SUFFIX       devel)
  endif()
endif()
set( XDISPATCH_LIBRARY_NAME		xdispatch)
set( XDISPATCH_SUMMARY			"Userspace implementation of grand central dispatch")
set( XDISPATCH_MAINTAINER		"Marius Zwicker")
set( XDISPATCH_LICENSE			"Apache License, Version 2.0")
set( XDISPATCH_DESCRIPTION		"Provides an userspace implementation of the grand central dispatch api introduced with Mac OS X 10.6. Additionally a platform independent C++ interface is included")
if( NOT IS_A_RELEASE )
  set( XDISPATCH_VERSION 			${XDISPATCH_VERSION_MAJOR}.${XDISPATCH_VERSION_MINOR}.${XDISPATCH_VERSION_PATCH}~${XDISPATCH_VERSION_SUFFIX})
  set( XDISPATCH_DLL_VERSION 		${XDISPATCH_VERSION_MAJOR},${XDISPATCH_VERSION_MINOR},${XDISPATCH_VERSION_PATCH},${XDISPATCH_WC_REVISION} )
else()
  set( XDISPATCH_VERSION 			${XDISPATCH_VERSION_MAJOR}.${XDISPATCH_VERSION_MINOR}.${XDISPATCH_VERSION_PATCH})
  set( XDISPATCH_DLL_VERSION 		${XDISPATCH_VERSION_MAJOR},${XDISPATCH_VERSION_MINOR},${XDISPATCH_VERSION_PATCH},0)
endif()
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "userspace implementation of grand central dispatch")
set( CPACK_PACKAGE_VENDOR 		"www.mlba-team.de")
set( CPACK_RESOURCE_FILE_LICENSE 	"${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set( CPACK_RESOURCE_FILE_README 	"${CMAKE_CURRENT_SOURCE_DIR}/README.dox")
set( CPACK_PACKAGE_DESCRIPTION_FILE     "${CMAKE_CURRENT_SOURCE_DIR}/README.dox")

# components xdispatch is built of
# depending on the platform, different components are needed
if(APPLE)
	if( XDISPATCH_QT_SUPPORT )
    	set( CPACK_COMPONENTS_ALL libxdispatch libQtDispatch xdispatch-doc )
    else()
	    set( CPACK_COMPONENTS_ALL libxdispatch xdispatch-doc )
	endif()
else()
    if(UNIX AND NOT APPLE)
		if( XDISPATCH_QT_SUPPORT )
			set( CPACK_COMPONENTS_ALL libdispatch libdispatch-dev libxdispatch libxdispatch-dev libQtDispatch libQtDispatch-dev xdispatch-doc )
		else()
			set( CPACK_COMPONENTS_ALL libdispatch libdispatch-dev libxdispatch libxdispatch-dev xdispatch-doc )
		endif()    
    else() # windows
		if( XDISPATCH_QT_SUPPORT )
			set( CPACK_COMPONENTS_ALL libdispatch libxdispatch libQtDispatch xdispatch-doc )
		else()
			set( CPACK_COMPONENTS_ALL libdispatch libxdispatch xdispatch-doc )
		endif()     
    endif()
endif()

# component descriptions and dependencies
set( CPACK_COMPONENT_LIBDISPATCH_DISPLAY_NAME "Grand Central Dispatch")
set( CPACK_COMPONENT_LIBDISPATCH_DESCRIPTION "user space implementation of the Grand Central Dispatch API")
set( CPACK_COMPONENT_LIBDISPATCH-DEV_DISPLAY_NAME "Grand Central Dispatch - Development Files")
set( CPACK_COMPONENT_LIBDISPATCH-DEV_DESCRIPTION "user space implementation of the Grand Central Dispatch API")
set( CPACK_COMPONENT_LIBDISPATCH-DEV_DEPENDS libdispatch)
set( CPACK_COMPONENT_LIBXDISPATCH_DISPLAY_NAME "XDispatch")
set( CPACK_COMPONENT_LIBXDISPATCH_DESCRIPTION "C++ API for Grand Central Dispatch")
if(APPLE)
    set( CPACK_COMPONENT_LIBXDISPATCH_DEPENDS )
else()
    set( CPACK_COMPONENT_LIBXDISPATCH_DEPENDS libdispatch )
endif()
set( CPACK_COMPONENT_LIBXDISPATCH-DEV_DISPLAY_NAME "XDispatch - Development Files")
set( CPACK_COMPONENT_LIBXDISPATCH-DEV_DESCRIPTION "C++ API for Grand Central Dispatch")
set( CPACK_COMPONENT_LIBXDISPATCH-DEV_DEPENDS libxdispatch)
if( XDISPATCH_QT_SUPPORT )
	set( CPACK_COMPONENT_LIBQTDISPATCH_DISPLAY_NAME "QtDispatch")
	set( CPACK_COMPONENT_LIBQTDISPATCH_DESCRIPTION "Provides integration of Grand Central Dispatch into the Qt Event-Loop. Additionally a QThreadPool compatible API to Grand Central Dispatch is provided")
	set( CPACK_COMPONENT_LIBQTDISPATCH_DEPENDS libxdispatch)
	set( CPACK_COMPONENT_LIBQTDISPATCH-DEV_DISPLAY_NAME "QtDispatch - Development Files")
	set( CPACK_COMPONENT_LIBQTDISPATCH-DEV_DESCRIPTION "Provides integration of Grand Central Dispatch into the Qt Event-Loop. Additionally a QThreadPool compatible API to Grand Central Dispatch is provided")
	set( CPACK_COMPONENT_LIBQTDISPATCH-DEV_DEPENDS libQtDispatch)
endif()
set( CPACK_COMPONENT_XDISPATCH-DOC_DISPLAY_NAME "API Documentation")
set( CPACK_COMPONENT_XDISPATCH-DOC_DESCRIPTION "Documentation for libxdispatch, libdispatch and libqtdispatch. API Reference, tutorials and further information. Will be installed to /Library/Documentation on Mac OS and to /usr/share/doc on other Unix platforms")

# variables modifying the package name
if( WIN32 )
	if( MINGW )
		set( PACKAGE_NAME "mingw" )
	else()
		set( PACKAGE_NAME "${CMAKE_GENERATOR}" )
	endif()
elseif( APPLE )
	set( PACKAGE_NAME "${CMAKE_SYSTEM}" )
else()
	set( PACKAGE_NAME "${CMAKE_SYSTEM_NAME}" )
endif()
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set( PACKAGE_NAME "${PACKAGE_NAME}_Debug" )
endif()


# CPack constants built from the other variables
set( CPACK_PACKAGE_VERSION_MAJOR 	"${XDISPATCH_VERSION_MAJOR}")
set( CPACK_PACKAGE_VERSION_MINOR 	"${XDISPATCH_VERSION_MINOR}")
set( CPACK_PACKAGE_VERSION_PATCH 	"${XDISPATCH_VERSION_PATCH}")
set( CPACK_PACKAGE_VERSION 		"${XDISPATCH_VERSION}")
set( CPACK_PACKAGE_FILE_NAME 		"${XDISPATCH_LIBRARY_NAME}_${CPACK_PACKAGE_VERSION}_${PACKAGE_NAME}_${PACKAGE_ARCH}")


# some debugging output
message("-- version information for '${XDISPATCH_VERSION}'")
