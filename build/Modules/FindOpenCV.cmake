
FIND_PATH(
	OPENCV_CFG_PATH
	NAMES
	OpenCVConfig.cmake
	HINTS
	/usr/local/share/OpenCV
	/usr/share/OpenCV
	$ENV{OpenCV_DIR}
)

include(FindPackageHandleStandardArgs)

if( OPENCV_CFG_PATH )
	## Include the standard CMake script
	include("${OPENCV_CFG_PATH}/OpenCVConfig.cmake")

    ## Search for a specific version
    set(CVLIB_SUFFIX "${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}")

	set( OPENCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS} )
	set( OPENCV_LIBRARIES ${OpenCV_LIBRARIES} ${OpenCV_LIBS})
else()
	message("-- Could not find OpenCV, you might have to set an environment")
	message("   variable 'OpenCV_DIR' pointing to the directory of your")
	message("   installation containing the file 'OpenCVConfig.cmake'")
endif()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	OpenCV
	DEFAULT_MSG
	OPENCV_INCLUDE_DIRS
	OPENCV_LIBRARIES
)

MARK_AS_ADVANCED(
	OPENCV_INCLUDE_DIRS
	OPENCV_LIBRARIES
) 

