########################################################################
#
#	DOCGEN.CMAKE
#
# 	This file adds an optional build step
#	to generate doxygen documentation
#	(c) 2010 Marius Zwicker
#
########################################################################

cmake_minimum_required(VERSION 2.6)

find_package(Doxygen)

file(GLOB PNG ${CMAKE_CURRENT_SOURCE_DIR}/docs/*.png)

set(DOXYGEN_DOT_FOUND NO)
set(DOXYGEN_DOT_EXECUTABLE "")

if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
				${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
	
	if(NOT WIN32)
		add_custom_target(Docs
		# run doxygen
		    ${DOXYGEN_EXECUTABLE} Doxyfile
		# copy additional files into the new directories
			COMMAND cp -f ${PNG} ${CMAKE_CURRENT_SOURCE_DIR}/Build/Docs/html
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
		
		if(APPLE)
			# create a docset for XCode
			add_custom_command(TARGET Docs POST_BUILD
				COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/Build/Docs/html
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			)
			
			# create a Qt Assistant Docset
        	add_custom_command(TARGET Docs POST_BUILD
                COMMAND ${QT_BINARY_DIR}/qhelpgenerator index.qhp -o com.mlba.projektx.qch
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Build/Docs/html
                COMMENT "Creating QT Docset"
        	)
		endif()
	else()
		file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/docs/*.png" PNG_IN)
		file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Build/Docs/html" PNG_OUT)
		add_custom_target(Docs
		# run doxygen
			${DOXYGEN_EXECUTABLE} Doxyfile
		# copy additional files into the new directories
			COMMAND copy /Y ${PNG_IN} ${PNG_OUT}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
		
		# create a Qt Assistant Docset
		add_custom_command(TARGET Docs POST_BUILD
				COMMAND ${QT_BINARY_DIR}/qhelpgenerator index.qhp -o com.mlba.projektx.qch
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Build/Docs/html
				COMMENT "Creating QT Docset"
		)
	endif()
	message("-- Found Doxygen, adding Documentation")
else()
	message("-- No Doxygen found, skipping Documentation")
endif()
