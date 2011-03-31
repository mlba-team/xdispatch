# tests for xdispatch

mz_add_library(munit munit)

# define file groups
file(GLOB TESTSRCS *.c *.h *.cpp)

# grouping
source_group(tests FILES ${TESTSRCS})

if(QT4_FOUND)
	# test cases for Qt Interface
	file(GLOB QTESTHEADER Qt_*.cpp)
	qt4_wrap_cpp(TESTMOC ${QTESTHEADER})
	
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
add_executable(xdispatch_tests ${TESTSRCS})

# link this target with all needed libraries
target_link_libraries(xdispatch_tests xdispatch ${LIB} munit)
mz_target_props(xdispatch_tests)
