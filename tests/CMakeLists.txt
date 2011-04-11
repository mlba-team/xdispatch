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
    set(TESTMOC ${TESTMOC} ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_group.moc)
	
    #message("### COOL ${QTESTHEADER}")

    set(LIB ${QT_LIBRARIES} QtDispatch)
endif()

# includes
INCLUDE_DIRECTORIES(
	${QT_QTCORE_INCLUDE_DIR}
 	${QT_QTTEST_INCLUDE_DIR}
 	${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# add new exec target
add_executable(xdispatch_tests ${TESTSRCS} ${SHIMS} ${TESTMOC} ${QTESTHEADER} tests.rc)

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
endif()

# link this target with all needed libraries
target_link_libraries(xdispatch_tests xdispatch ${LIB} munit ${CMAKE_CURRENT_BINARY_DIR}/tests.res)
mz_target_props(xdispatch_tests)
