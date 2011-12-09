
# Variables referring to the system the packaging took place at
if(MZ_64BIT)
    set( PACKAGE_ARCH 			"amd64")
else()
    set( PACKAGE_ARCH 			"i386")
endif()
if(MZ_IS_CLANG)
    set( PACKAGE_COMPILER 		"clang")
else()
    if(MZ_IS_VS)
        set( PACKAGE_COMPILER 	"msvc")
    else()
        set( PACKAGE_COMPILER 	"gcc")
    endif()
endif()

# Variables describing the version and functionality of 
# libxdispatch will be updated on each new version release

set( XDISPATCH_VERSION_MAJOR 	0)
set( XDISPATCH_VERSION_MINOR 	7)
set( XDISPATCH_VERSION_PATCH 	0)
set( XDISPATCH_VERSION_SUFFIX 	devel)
set( XDISPATCH_LIBRARY_NAME		xdispatch)
set( XDISPATCH_SUMMARY			"Userspace implementation of grand central dispatch")
set( XDISPATCH_MAINTAINER		"Marius Zwicker")
set( XDISPATCH_LICENSE			"Apache License, Version 2.0")
set( XDISPATCH_DESCRIPTION		"Provides an userspace implementation of the grand central dispatch api introduced with Mac OS X 10.6. Additionally a platform independent C++ interface is included")
set( XDISPATCH_VERSION 			${XDISPATCH_VERSION_MAJOR}.${XDISPATCH_VERSION_MINOR}.${XDISPATCH_VERSION_PATCH}${XDISPATCH_VERSION_SUFFIX})
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "userspace implementation of grand central dispatch")
set( CPACK_PACKAGE_VENDOR 		"www.mlba-team.de")
set( CPACK_RESOURCE_FILE_LICENSE 	"${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set( CPACK_RESOURCE_FILE_README 	"${CMAKE_CURRENT_SOURCE_DIR}/README.dox")
set( CPACK_PACKAGE_DESCRIPTION_FILE     "${CMAKE_CURRENT_SOURCE_DIR}/README.dox")

# The components xdispatch is built of
set( CPACK_COMPONENTS_ALL libdispatch libdispatch-dev libxdispatch libxdispatch-dev libQtDispatch libQtDispatch-dev )
set( CPACK_COMPONENT_LIBDISPATCH_DISPLAY_NAME "Grand Central Dispatch")
set( CPACK_COMPONENT_LIBDISPATCH_DESCRIPTION "user space implementation of the Grand Central Dispatch API")
set( CPACK_COMPONENT_LIBDISPATCH-DEV_DISPLAY_NAME "Grand Central Dispatch - Development Files")
set( CPACK_COMPONENT_LIBDISPATCH-DEV_DESCRIPTION "user space implementation of the Grand Central Dispatch API")
set( CPACK_COMPONENT_LIBDISPATCH-DEV_DEPENDS libdispatch)
set( CPACK_COMPONENT_LIBXDISPATCH_DISPLAY_NAME "XDispatch")
set( CPACK_COMPONENT_LIBXDISPATCH_DESCRIPTION "C++ API for Grand Central Dispatch")
set( CPACK_COMPONENT_LIBXDISPATCH_DEPENDS libdispatch)
set( CPACK_COMPONENT_LIBXDISPATCH-DEV_DISPLAY_NAME "XDispatch - Development Files")
set( CPACK_COMPONENT_LIBXDISPATCH-DEV_DESCRIPTION "C++ API for Grand Central Dispatch")
set( CPACK_COMPONENT_LIBXDISPATCH-DEV_DEPENDS libxdispatch)
set( CPACK_COMPONENT_LIBQTDISPATCH_DISPLAY_NAME "QtDispatch")
set( CPACK_COMPONENT_LIBQTDISPATCH_DESCRIPTION "Provides integration of Grand Central Dispatch into the Qt Event-Loop. Additionally a QThreadPool compatible API to Grand Central Dispatch is provided")
set( CPACK_COMPONENT_LIBQTDISPATCH_DEPENDS libxdispatch)
set( CPACK_COMPONENT_LIBQTDISPATCH-DEV_DISPLAY_NAME "QtDispatch - Development Files")
set( CPACK_COMPONENT_LIBQTDISPATCH-DEV_DESCRIPTION "Provides integration of Grand Central Dispatch into the Qt Event-Loop. Additionally a QThreadPool compatible API to Grand Central Dispatch is provided")
set( CPACK_COMPONENT_LIBQTDISPATCH-DEV_DEPENDS libQtDispatch)



# CPack constants built from the other variables
set( CPACK_PACKAGE_VERSION_MAJOR 	"${XDISPATCH_VERSION_MAJOR}")
set( CPACK_PACKAGE_VERSION_MINOR 	"${XDISPATCH_VERSION_MINOR}")
set( CPACK_PACKAGE_VERSION_PATCH 	"${XDISPATCH_VERSION_PATCH}")
set( CPACK_PACKAGE_VERSION 		"${XDISPATCH_VERSION}")
set( CPACK_PACKAGE_FILE_NAME 		"${XDISPATCH_LIBRARY_NAME}_${CPACK_PACKAGE_VERSION}_${CMAKE_SYSTEM_NAME}_${PACKAGE_COMPILER}_${PACKAGE_ARCH}")


# some debugging output
message("-- version information for '${XDISPATCH_VERSION}'")
