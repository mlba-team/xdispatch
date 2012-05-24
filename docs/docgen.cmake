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
		configure_file(
			${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
			${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
		)

        # add to the default target
        if( MZ_IS_RELEASE )
            set( DOCS_DEFAULT_TARGET ALL )
        endif()
	
        add_custom_target(xdispatch_docs ${DOCS_DEFAULT_TARGET}
		# delete old docs
			${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/docs/
			COMMAND ${DOXYGEN_EXECUTABLE}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
		# copy additional files into the new directories
		foreach(_png ${PNG})
			add_custom_command( TARGET xdispatch_docs POST_BUILD
				COMMAND ${CMAKE_COMMAND}
				ARGS -E copy ${_png} ${CMAKE_CURRENT_BINARY_DIR}/docs/html
			)
		endforeach(_png)

        # the install target for the docs
        if(APPLE)
            install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html
                        DESTINATION "Library/Documentation/xdispatch"
                        COMPONENT xdispatch-doc
                        PATTERN "*.svn" EXCLUDE)
            install(FILES LICENSE.rtf NOTICE README.rtf BUGS
            			DESTINATION "Library/Documentation/xdispatch"
            			COMPONENT xdispatch-doc)
        elseif(UNIX)
            install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html
                        DESTINATION share/doc/xdispatch-doc
                        COMPONENT xdispatch-doc
                        PATTERN "*.svn" EXCLUDE)
        else()
            install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs
                        DESTINATION doc
                        COMPONENT xdispatch-doc
                        PATTERN "*.svn" EXCLUDE)
        endif()

	message("-- Found Doxygen, adding Documentation for xdispatch (make xdispatch_docs)")
else()
	message("-- No Doxygen found, skipping Documentation for xdispatch")
endif()
