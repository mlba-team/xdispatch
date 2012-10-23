# tests for xdispatch
mz_add_library(munit munit)

# test available features
try_compile( HAS_BLOCKS
    ${CMAKE_CURRENT_BINARY_DIR}/Blocks
    ${CMAKE_CURRENT_SOURCE_DIR}/checks/blocks.c
    OUTPUT_VARIABLE HAS_BLOCKS_OUTPUT
)
try_compile( HAS_LAMBDAS
    ${CMAKE_CURRENT_BINARY_DIR}/Lambdas
    ${CMAKE_CURRENT_SOURCE_DIR}/checks/lambdas.cpp
    OUTPUT_VARIABLE HAS_LAMBDAS_OUTPUT
)
if(HAS_LAMBDAS)
    mz_message("Testing xdispatch lambda support")
endif()
if(HAS_BLOCKS)
    mz_message("Testing xdispatch block support")
#else()
    #mz_message("${HAS_BLOCKS_OUTPUT}")
endif()

# define file groups

file(GLOB TEST_DISPATCH
	dispatch_*.c
	dispatch_*.cpp
	tests.h 
	atomic_operations.c
	posix_api.c
	pthread_api.c
	queue_finalizer.c
)
file(GLOB TEST_DISPATCH_BLOCKS dispatch_*_blocks.c)
file(GLOB TEST_DISPATCH_LAMBDA dispatch_*_lambda.cpp)
if(NOT HAS_BLOCKS)
    list(REMOVE_ITEM TEST_DISPATCH ${TEST_DISPATCH_BLOCKS})
endif()
if(HAS_BLOCKS OR NOT HAS_LAMBDAS)
    list(REMOVE_ITEM TEST_DISPATCH ${TEST_DISPATCH_LAMBDA})
endif()

file(GLOB TEST_XDISPATCH 
	cxx_*.cpp
	cxx_tests.h
	cross_blocks.cpp
)
file(GLOB TEST_XDISPATCH_BLOCKS cxx_*_blocks.cpp)
file(GLOB TEST_XDISPATCH_LAMBDA cxx_*_lambda.cpp)
if(NOT HAS_LAMBDAS)
    list(REMOVE_ITEM TEST_XDISPATCH ${TEST_XDISPATCH_LAMBDA})
endif()
if(NOT HAS_BLOCKS)
    list(REMOVE_ITEM TEST_XDISPATCH ${TEST_XDISPATCH_BLOCKS})
endif()

file(GLOB SHIMS shims/*.c shims/*.h)


if(XDISPATCH_QT_SUPPORT)
    # test cases for Qt Interface
    file(GLOB TEST_QT Qt_*.cpp Qt_*.h)
    file(GLOB TEST_QT_LAMBDA Qt_*_lambda.cpp)
    file(GLOB TEST_QT_BLOCKS Qt_*_blocks.cpp)
    if(NOT HAS_LAMBDAS)
        list(REMOVE_ITEM TEST_QT ${TEST_QT_LAMBDA})
    endif()
    if(NOT HAS_BLOCKS)
        list(REMOVE_ITEM TEST_QT ${TEST_QT_BLOCKS})
    endif()
	
    mz_add_definition(XDISPATCH_QT_SUPPORT=1)

    # we need to manually define mocced files
    qt4_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qt_dispatch_group_blocks.cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_group_blocks.moc)
    qt4_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qt_dispatch_group_lambda.cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_group_lambda.moc)
    qt4_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qt_dispatch_source_signal_blocks.cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_source_signal_blocks.moc)
    qt4_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qt_dispatch_source_signal_lambda.cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_source_signal_lambda.moc)
    #qt4_generate_moc(${CMAKE_CURRENT_SOURCE_DIR}/Qt_dispatch_source_device.cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_source_device.moc)
    set(TESTMOC ${TESTMOC}
        ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_group_lambda.moc
        ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_group_blocks.moc
        ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_source_signal_lambda.moc
        ${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_source_signal_blocks.moc
        #${CMAKE_CURRENT_BINARY_DIR}/moc_Qt_dispatch_source_device.moc
    )

    #message("### COOL ${TEST_QTDISPATCH}")

    set(TEST_LIB ${TEST_LIB} ${QT_LIBRARIES} QtDispatch)
	
    include_directories(
        ${QT_QTCORE_INCLUDE_DIR}
        ${QT_QTTEST_INCLUDE_DIR}
    )
endif()

# includes
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

if(MINGW)
    set(RES_FILES tests.rc)
endif()

if(NOT HAVE_NATIVE_DISPATCH_H)
    set(TEST_LIB ${TEST_LIB} dispatch)
endif()

# add new exec target
if( IOS )
    add_library(xdispatch_tests STATIC main_ios.c ${TEST_DISPATCH} ${TEST_XDISPATCH} ${SHIMS} ${TESTMOC} ${TEST_QT} ${RES_FILES})
else()
    add_executable(xdispatch_tests main.c ${TEST_DISPATCH} ${TEST_XDISPATCH} ${SHIMS} ${TESTMOC} ${TEST_QT} ${RES_FILES})
endif()

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
add_test( NAME atomic_operations COMMAND ${TEST_EXE} -n atomic_operations )
add_test( NAME pthread_api COMMAND ${TEST_EXE} -n pthread_api )
add_test( NAME test_dispatch_once COMMAND ${TEST_EXE} -n test_dispatch_once )
add_test( NAME dispatch_api COMMAND ${TEST_EXE} -n dispatch_api )
add_test( NAME dispatch_simpleFunction COMMAND ${TEST_EXE} -n dispatch_simpleFunction )
add_test( NAME dispatch_test_sync COMMAND ${TEST_EXE} -n dispatch_test_sync )
add_test( NAME dispatch_testDebug COMMAND ${TEST_EXE} -n dispatch_testDebug )
add_test( NAME dispatch_apply_function COMMAND ${TEST_EXE} -n dispatch_apply_function )
add_test( NAME dispatch_priority COMMAND ${TEST_EXE} -n dispatch_priority )
add_test( NAME dispatch_priority2 COMMAND ${TEST_EXE} -n dispatch_priority2 )
add_test( NAME dispatch_cascade COMMAND ${TEST_EXE} -n dispatch_cascade )
add_test( NAME dispatch_apply_serialqueue COMMAND ${TEST_EXE} -n dispatch_apply_serialqueue )
add_test( NAME dispatch_after_function COMMAND ${TEST_EXE} -n dispatch_after_function )
add_test( NAME test_queue_finalizer COMMAND ${TEST_EXE} -n test_queue_finalizer )
add_test( NAME dispatch_memory_use COMMAND ${TEST_EXE} -n dispatch_memory_use )
add_test( NAME dispatch_group_function COMMAND ${TEST_EXE} -n dispatch_group_function )
add_test( NAME dispatch_plusplus_blocks COMMAND ${TEST_EXE} -n dispatch_plusplus_blocks )
add_test( NAME dispatch_apply_blocks COMMAND ${TEST_EXE} -n dispatch_apply_blocks )
add_test( NAME dispatch_after_blocks COMMAND ${TEST_EXE} -n dispatch_after_blocks )
add_test( NAME dispatch_group_blocks COMMAND ${TEST_EXE} -n dispatch_group_blocks )
add_test( NAME dispatch_pingpong_blocks COMMAND ${TEST_EXE} -n dispatch_pingpong_blocks )
add_test( NAME dispatch_semaphore_blocks COMMAND ${TEST_EXE} -n dispatch_semaphore_blocks )
add_test( NAME dispatch_drift_blocks COMMAND ${TEST_EXE} -n dispatch_drift_blocks )
add_test( NAME cross_blocks COMMAND ${TEST_EXE} -n cross_blocks)
add_test( NAME dispatch_apply_lambda COMMAND ${TEST_EXE} -n dispatch_apply_lambda )
add_test( NAME dispatch_after_lambda COMMAND ${TEST_EXE} -n dispatch_after_lambda )
add_test( NAME dispatch_group_lambda COMMAND ${TEST_EXE} -n dispatch_group_lambda )
add_test( NAME dispatch_pingpong_lambda COMMAND ${TEST_EXE} -n dispatch_pingpong_lambda )
add_test( NAME dispatch_semaphore_lambda COMMAND ${TEST_EXE} -n dispatch_semaphore_lambda )
add_test( NAME dispatch_timer_bit31_lambda COMMAND ${TEST_EXE} -n dispatch_timer_bit31_lambda )
add_test( NAME dispatch_timer_bit63_lambda COMMAND ${TEST_EXE} -n dispatch_timer_bit63_lambda )
add_test( NAME cxx_dispatch_debug COMMAND ${TEST_EXE} -n cxx_dispatch_debug )
add_test( NAME cxx_dispatch_group COMMAND ${TEST_EXE} -n cxx_dispatch_group )
add_test( NAME cxx_dispatch_mainqueue COMMAND ${TEST_EXE} -n cxx_dispatch_mainqueue )
add_test( NAME cxx_dispatch_semaphore COMMAND ${TEST_EXE} -n cxx_dispatch_semaphore )
add_test( NAME cxx_dispatch_current COMMAND ${TEST_EXE} -n cxx_dispatch_current )
add_test( NAME cxx_dispatch_timer COMMAND ${TEST_EXE} -n cxx_dispatch_timer )
add_test( NAME cxx_free_lambda COMMAND ${TEST_EXE} -n cxx_free_lambda )
add_test( NAME cxx_dispatch_once1 COMMAND ${TEST_EXE} -n cxx_dispatch_once1 )
add_test( NAME cxx_synchronized COMMAND ${TEST_EXE} -n cxx_synchronized )
add_test( NAME cxx_dispatch_fibo COMMAND ${TEST_EXE} -n cxx_dispatch_fibo )
add_test( NAME cxx_dispatch_cascade_blocks COMMAND ${TEST_EXE} -n cxx_dispatch_cascade_blocks )
add_test( NAME cxx_dispatch_group_blocks COMMAND ${TEST_EXE} -n cxx_dispatch_group_blocks )
add_test( NAME cxx_dispatch_once2_blocks COMMAND ${TEST_EXE} -n cxx_dispatch_once2_blocks )
add_test( NAME cxx_dispatch_once3_blocks COMMAND ${TEST_EXE} -n cxx_dispatch_once3_blocks )
add_test( NAME cxx_dispatch_queue_blocks COMMAND ${TEST_EXE} -n cxx_dispatch_queue_blocks )
add_test( NAME cxx_dispatch_read_blocks COMMAND ${TEST_EXE} -n cxx_dispatch_read_blocks )
add_test( NAME cxx_dispatch_serialqueue_blocks COMMAND ${TEST_EXE} -n cxx_dispatch_serialqueue_blocks )
add_test( NAME cxx_dispatch_source_blocks COMMAND ${TEST_EXE} -n cxx_dispatch_source_blocks )
add_test( NAME cxx_dispatch_cascade_lambda COMMAND ${TEST_EXE} -n cxx_dispatch_cascade_lambda )
add_test( NAME cxx_dispatch_group_lambda COMMAND ${TEST_EXE} -n cxx_dispatch_group_lambda )
add_test( NAME cxx_dispatch_once2_lambda COMMAND ${TEST_EXE} -n cxx_dispatch_once2_lambda )
add_test( NAME cxx_dispatch_queue_lambda COMMAND ${TEST_EXE} -n cxx_dispatch_queue_lambda )
add_test( NAME cxx_dispatch_read_lambda COMMAND ${TEST_EXE} -n cxx_dispatch_read_lambda )
add_test( NAME cxx_dispatch_proc_lambda COMMAND ${TEST_EXE} -n cxx_dispatch_proc_lambda )
add_test( NAME cxx_dispatch_serialqueue_lambda COMMAND ${TEST_EXE} -n cxx_dispatch_serialqueue_lambda )
add_test( NAME cxx_dispatch_source_lambda COMMAND ${TEST_EXE} -n cxx_dispatch_source_lambda )
add_test( NAME Qt_dispatch_fibo COMMAND ${TEST_EXE} -n Qt_dispatch_fibo )
add_test( NAME Qt_dispatch_debug COMMAND ${TEST_EXE} -n Qt_dispatch_debug )
add_test( NAME Qt_dispatch_source_device COMMAND ${TEST_EXE} -n Qt_dispatch_source_device )
add_test( NAME Qt_dispatch_timer COMMAND ${TEST_EXE} -n Qt_dispatch_timer )
add_test( NAME Qt_dispatch_group1 COMMAND ${TEST_EXE} -n Qt_dispatch_group1 )
add_test( NAME Qt_runnable_lambda COMMAND ${TEST_EXE} -n Qt_runnable_lambda )
add_test( NAME Qt_dispatch_queue_lambda COMMAND ${TEST_EXE} -n Qt_dispatch_queue_lambda )
add_test( NAME Qt_dispatch_mainqueue_lambda COMMAND ${TEST_EXE} -n Qt_dispatch_mainqueue_lambda )
add_test( NAME Qt_dispatch_cascade_lambda COMMAND ${TEST_EXE} -n Qt_dispatch_cascade_lambda )
add_test( NAME Qt_dispatch_after_lambda COMMAND ${TEST_EXE} -n Qt_dispatch_after_lambda )
add_test( NAME Qt_dispatch_group2_lambda COMMAND ${TEST_EXE} -n Qt_dispatch_group2_lambda )
add_test( NAME Qt_dispatch_serialqueue_lambda COMMAND ${TEST_EXE} -n Qt_dispatch_serialqueue_lambda )
add_test( NAME Qt_dispatch_semaphore_lambda COMMAND ${TEST_EXE} -n Qt_dispatch_semaphore_lambda )
add_test( NAME Qt_dispatch_current_lambda COMMAND ${TEST_EXE} -n Qt_dispatch_current_lambda )
add_test( NAME Qt_dispatch_source_signal_lambda COMMAND ${TEST_EXE} -n Qt_dispatch_source_signal_lambda )
add_test( NAME Qt_dispatch_source_network_lambda COMMAND ${TEST_EXE} -n Qt_dispatch_source_network_lambda )
add_test( NAME Qt_free_lambda COMMAND ${TEST_EXE} -n Qt_free_lambda )
add_test( NAME Qt_early_dispatch1_lambda COMMAND ${TEST_EXE} -n Qt_early_dispatch1_lambda )
add_test( NAME Qt_early_dispatch2_lambda COMMAND ${TEST_EXE} -n Qt_early_dispatch2_lambda )
add_test( NAME Qt_runnable_blocks COMMAND ${TEST_EXE} -n Qt_runnable_blocks )
add_test( NAME Qt_dispatch_queue_blocks COMMAND ${TEST_EXE} -n Qt_dispatch_queue_blocks )
add_test( NAME Qt_dispatch_mainqueue_blocks COMMAND ${TEST_EXE} -n Qt_dispatch_mainqueue_blocks )
add_test( NAME Qt_dispatch_cascade_blocks COMMAND ${TEST_EXE} -n Qt_dispatch_cascade_blocks )
add_test( NAME Qt_dispatch_after_blocks COMMAND ${TEST_EXE} -n Qt_dispatch_after_blocks )
add_test( NAME Qt_dispatch_group2_blocks COMMAND ${TEST_EXE} -n Qt_dispatch_group2_blocks )
add_test( NAME Qt_dispatch_serialqueue_blocks COMMAND ${TEST_EXE} -n Qt_dispatch_serialqueue_blocks )
add_test( NAME Qt_dispatch_semaphore_blocks COMMAND ${TEST_EXE} -n Qt_dispatch_semaphore_blocks )
add_test( NAME Qt_dispatch_current_blocks COMMAND ${TEST_EXE} -n Qt_dispatch_current_blocks )
add_test( NAME Qt_dispatch_source_signal_blocks COMMAND ${TEST_EXE} -n Qt_dispatch_source_signal_blocks )
add_test( NAME Qt_dispatch_source_network_blocks COMMAND ${TEST_EXE} -n Qt_dispatch_source_network_blocks )
add_test( NAME Qt_free_blocks COMMAND ${TEST_EXE} -n Qt_free_blocks )
add_test( NAME Qt_early_dispatch1_blocks COMMAND ${TEST_EXE} -n Qt_early_dispatch1_blocks )
add_test( NAME Qt_early_dispatch2_blocks COMMAND ${TEST_EXE} -n Qt_early_dispatch2_blocks )

