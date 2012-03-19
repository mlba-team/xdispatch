# tests for xdispatch
mz_add_library(munit munit)

# xdispatch tests are opt-in
option(BUILD_XDISPATCH_TESTS "Enable to build the xdispatch test suite" OFF)
if( BUILD_XDISPATCH_TESTS)

    # define file groups
    file(GLOB TESTSRCS *.c *.h cxx_*.cpp dispatch_*.cpp cross_blocks.cpp)
    file(GLOB SHIMS shims/*.c shims/*.h)

    # grouping
    source_group(tests FILES ${TESTSRCS})
    source_group(shims FILES ${SHIMS})

    if(XDISPATCH_QT_SUPPORT)
	    # test cases for Qt Interface
	    file(GLOB QTESTHEADER Qt_*.cpp)
		
		mz_add_definition(XDISPATCH_QT_SUPPORT=1)

        # we need to manually define mocced files
        qt4_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qt_dispatch_group.cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_group.moc)
        qt4_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qt_dispatch_source_signal.cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_source_signal.moc)
        #qt4_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qt_dispatch_source_device.cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_source_device.moc)
        set(TESTMOC ${TESTMOC} 
		    ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_group.moc
		    ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_source_signal.moc
                    #${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_source_device.moc
	    )
	
        #message("### COOL ${QTESTHEADER}")

        set(TEST_LIB ${TEST_LIB} ${QT_LIBRARIES} QtDispatch)
    endif()

    # includes
    INCLUDE_DIRECTORIES(
	    ${QT_QTCORE_INCLUDE_DIR}
     	${QT_QTTEST_INCLUDE_DIR}
     	${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/../include
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

    # list all available tests
    set( TEST_EXE xdispatch_tests )
    add_test( NAME atomic_operations COMMAND ${TEST_EXE} -t 1 )
    add_test( NAME pthread_api COMMAND ${TEST_EXE} -t 2 )
    add_test( NAME dispatch_api COMMAND ${TEST_EXE} -t 3 )
    add_test( NAME dispatch_simpleFunction COMMAND ${TEST_EXE} -t 4 )
    add_test( NAME dispatch_test_sync COMMAND ${TEST_EXE} -t 5 )
    add_test( NAME dispatch_testDebug COMMAND ${TEST_EXE} -t 6 )
    add_test( NAME dispatch_apply_function COMMAND ${TEST_EXE} -t 7 )
    add_test( NAME dispatch_priority COMMAND ${TEST_EXE} -t 8 )
    add_test( NAME dispatch_priority2 COMMAND ${TEST_EXE} -t 9 )
    add_test( NAME dispatch_cascade COMMAND ${TEST_EXE} -t 10 )
    add_test( NAME dispatch_apply_serialqueue COMMAND ${TEST_EXE} -t 11 )
    add_test( NAME dispatch_after_function COMMAND ${TEST_EXE} -t 12 )
    add_test( NAME test_queue_finalizer COMMAND ${TEST_EXE} -t 13 )
    #add_test( NAME dispatch_starfish COMMAND ${TEST_EXE} -t 14 )
    add_test( NAME dispatch_memory_use COMMAND ${TEST_EXE} -t 15 )
    add_test( NAME dispatch_group_function COMMAND ${TEST_EXE} -t 16 )
    add_test( NAME cross_blocks COMMAND ${TEST_EXE} -t 17 )
    add_test( NAME dispatch_plusplus COMMAND ${TEST_EXE} -t 18 )
    add_test( NAME dispatch_apply_blocks COMMAND ${TEST_EXE} -t 19 )
    add_test( NAME dispatch_group_blocks COMMAND ${TEST_EXE} -t 20 )
    add_test( NAME dispatch_after_blocks COMMAND ${TEST_EXE} -t 21 )
    add_test( NAME dispatch_pingpong_blocks COMMAND ${TEST_EXE} -t 22 )
    add_test( NAME dispatch_semaphore COMMAND ${TEST_EXE} -t 23 )
    add_test( NAME dispatch_timer_bit31 COMMAND ${TEST_EXE} -t 24 )
    add_test( NAME dispatch_timer_bit63 COMMAND ${TEST_EXE} -t 25 )
    add_test( NAME dispatch_drift COMMAND ${TEST_EXE} -t 26 )
    add_test( NAME cxx_dispatch_debug COMMAND ${TEST_EXE} -t 27 )
    add_test( NAME cxx_dispatch_queue COMMAND ${TEST_EXE} -t 28 )
    add_test( NAME cxx_dispatch_mainqueue COMMAND ${TEST_EXE} -t 29 )
    add_test( NAME cxx_dispatch_cascade COMMAND ${TEST_EXE} -t 30 )
    add_test( NAME cxx_dispatch_group COMMAND ${TEST_EXE} -t 31 )
    add_test( NAME cxx_dispatch_serialqueue COMMAND ${TEST_EXE} -t 32 )
    add_test( NAME cxx_dispatch_semaphore COMMAND ${TEST_EXE} -t 33 )
    add_test( NAME cxx_dispatch_current COMMAND ${TEST_EXE} -t 34 )
    add_test( NAME cxx_dispatch_timer COMMAND ${TEST_EXE} -t 35 )
    add_test( NAME cxx_dispatch_source COMMAND ${TEST_EXE} -t 36 )
    add_test( NAME cxx_free_lambda COMMAND ${TEST_EXE} -t 37 )
    if( XDISPATCH_QT_SUPPORT )
        add_test( NAME Qt_runnable COMMAND ${TEST_EXE} -t 38 )
        add_test( NAME Qt_dispatch_debug COMMAND ${TEST_EXE} -t 39 )
        add_test( NAME Qt_dispatch_queue COMMAND ${TEST_EXE} -t 40 )
        add_test( NAME Qt_dispatch_mainqueue COMMAND ${TEST_EXE} -t 41 )
        add_test( NAME Qt_dispatch_cascade COMMAND ${TEST_EXE} -t 42 )
        add_test( NAME Qt_dispatch_after COMMAND ${TEST_EXE} -t 43 )
        add_test( NAME Qt_dispatch_group1 COMMAND ${TEST_EXE} -t 44 )
        add_test( NAME Qt_dispatch_group2 COMMAND ${TEST_EXE} -t 45 )
        add_test( NAME Qt_dispatch_serialqueue COMMAND ${TEST_EXE} -t 46 )
        add_test( NAME Qt_dispatch_semaphore COMMAND ${TEST_EXE} -t 47 )
        add_test( NAME Qt_dispatch_current COMMAND ${TEST_EXE} -t 48 )
        add_test( NAME Qt_dispatch_timer COMMAND ${TEST_EXE} -t 49 )
        add_test( NAME Qt_synchronized COMMAND ${TEST_EXE} -t 50 )
        add_test( NAME Qt_dispatch_source_signal COMMAND ${TEST_EXE} -t 51 )
        add_test( NAME Qt_dispatch_source_device COMMAND ${TEST_EXE} -t 52 )
        add_test( NAME Qt_dispatch_source_network COMMAND ${TEST_EXE} -t 53 )
        add_test( NAME Qt_free_lambda COMMAND ${TEST_EXE} -t 54 )
        add_test( NAME Qt_early_dispatch1 COMMAND ${TEST_EXE} -t 55 )
        add_test( NAME Qt_early_dispatch2 COMMAND ${TEST_EXE} -t 56 )
    endif()
    
endif()

