
file(GLOB QSRC *.h *.cpp)
file(GLOB QHEADER *.h)
file(GLOB QINCLUDE_H ../include/QtDispatch/*.h)
file(GLOB QINCLUDE ../include/QtDispatch/Q*)

# include qt headers (if found)	
INCLUDE_DIRECTORIES(
	${QT_QTCORE_INCLUDE_DIR}
)

# moc qt files
qt4_wrap_cpp(MOC ${QHEADER} ${QINCLUDE_H})

source_group(src FILES ${QSRC} ${QHEADER})
source_group(includes FILES ${QINCLUDE} ${QINCLUDE_H})

add_library(QtDispatch STATIC ${QSRC} ${MOC} ${QHEADER} ${QINCLUDE} ${QINCLUDE_H})
target_link_libraries(QtDispatch xdispatch ${QT_QTCORE_LIBRARY})

mz_target_props(QtDispatch)

