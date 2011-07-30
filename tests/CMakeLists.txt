# tests for xdispatch
mz_add_library(munit munit)

# define file groups
file(GLOB TESTSRCS *.c *.h cxx_*.cpp dispatch_*.cpp cross_blocks.cpp)
file(GLOB SHIMS shims/*.c shims/*.h)

# grouping
source_group(tests FILES ${TESTSRCS})
source_group(shims FILES ${SHIMS})

if(QT4_FOUND)
	# test cases for Qt Interface
	file(GLOB QTESTHEADER Qt_*.cpp)

    # we need to manually define mocced files
    qt4_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qt_dispatch_group.cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_group.moc)
	qt4_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qt_dispatch_source_signal.cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_source_signal.moc)
	qt4_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qt_dispatch_source_device.cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_source_device.moc)
    set(TESTMOC ${TESTMOC} 
		${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_group.moc
		${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_source_signal.moc
		${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_source_device.moc
	)
	
    #message("### COOL ${QTESTHEADER}")

    set(TEST_LIB ${TEST_LIB} ${QT_LIBRARIES} QtDispatch)
endif()

# includes
INCLUDE_DIRECTORIES(
	${QT_QTCORE_INCLUDE_DIR}
 	${QT_QTTEST_INCLUDE_DIR}
 	${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

if(MINGW)
	set(RES_FILES tests.rc)
endif()

if(NOT HAVE_NATIVE_DISPATCH_H)
	set(TEST_LIB ${TEST_LIB} dispatch)
endif()

# add new exec target
add_executable(xdispatch_tests ${TESTSRCS} ${SHIMS} ${TESTMOC} ${QTESTHEADER} ${RES_FILES})

# manifest (somehow we need one when using mingw - but only for this binary !?
if(MINGW)
    # create the manifest for the first time
    execute_process(COMMAND windres --input ${CMAKE_CURRENT_SOURCE_DIR}/tests.rc --output tests.res --output-format=coff
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    # and all other times automatically
    add_custom_command( TARGET xdispatch_tests PRE_LINK
                        COMMAND windres --input ${CMAKE_CURRENT_SOURCE_DIR}/tests.rc --output tests.res --output-format=coff
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
	set(RES_OBJECT ${CMAKE_CURRENT_BINARY_DIR}/tests.res)
endif()

# link this target with all needed libraries
target_link_libraries(xdispatch_tests xdispatch ${TEST_LIB} ${LIBS} munit ${RES_OBJECT})
mz_target_props(xdispatch_tests)
