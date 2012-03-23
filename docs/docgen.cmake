########################################################################
#
#	DOCGEN.CMAKE
#
# 	This file adds an optional build step
#	to generate doxygen documentation
#	(c) 2010 Marius Zwicker
#
########################################################################

cmake_minimum_required(VERSION 2.8)

find_package(Doxygen)

file(GLOB PNG ${CMAKE_CURRENT_SOURCE_DIR}/docs/*.png)
set(DOXYGEN_DOT_FOUND NO)
set(DOXYGEN_DOT_EXECUTABLE "")

if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
				${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
	
	add_custom_target(xdispatch_docs
		# delete old docs
			${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/docs/
		# run doxygen
			COMMAND ${DOXYGEN_EXECUTABLE}
		# copy additional files into the new directories
			COMMAND cp -f ${PNG} ${CMAKE_CURRENT_BINARY_DIR}/docs/html
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
		
	# create a Qt Assistant Docset
	add_custom_command(TARGET lemon_docs POST_BUILD
		COMMAND ${QT_BINARY_DIR}/qhelpgenerator index.qhp -o ${EXECUTABLE_OUTPUT_PATH}/com.mlba.lemon.qch
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html
		COMMENT "Creating QT Docset"
    )

	message("-- Found Doxygen, adding Documentation for lemon (make lemon_docs)")
else()
	message("-- No Doxygen found, skipping Documentation for lemon")
endif()