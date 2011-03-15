# tests for xdispatch

# define file groups
file(GLOB TESTSRCS *.c *.h *.cpp)
file(GLOB_RECURSE MUNIT munit/*.c munit/*.h)

# grouping
source_group(tests FILES ${TESTSRCS})
source_group(munit FILES ${MUNIT})

if(QT4_FOUND)
	# test cases f)r Qt Interface
	file(GLOB QTESTHEADER *.h)
	qt4_wrap_cpp(MOC ${QTESTHEADER})
endif()

# includes
INCLUDE_DIRECTORIES(
	${QT_QTCORE_INCLUDE_DIR}
	${QT_QTTEST_INCLUDE_DIR}
	../../
)

# add new exec target
add_executable(xdispatch_tests ${TESTSRCS} ${TESTMOC} ${MUNIT})

# link this target with all needed libraries
if(MZ_IS_VS)
 set(LIB pthreads-w32)
endif()

# link this target with all needed libraries
target_link_libraries(xdispatch_tests xdispatch ${QT_LIBRARIES} ${LIB})
mz_target_props(xdispatch_tests)
