

add_custom_target(
    ubuntu ./package/ubuntu/package.sh xdispatch-${XDISPATCH_VERSION}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

configure_file( 
    ${CMAKE_CURRENT_SOURCE_DIR}/package/ubuntu/changelog.in
    ${CMAKE_CURRENT_SOURCE_DIR}/package/ubuntu/changelog
)

	
message("-- Configured generator 'DEB (Ubuntu)'")
set( XDISPATCH_CPACK ${XDISPATCH_CPACK} DEB )
