
file(GLOB QTEXT *.h *.cpp ../xdispatch/QtDispatch/*)
file(GLOB QHEADER *.h)
file(GLOB QINCLUDE_H ../xdispatch/QtDispatch/*.h)
file(GLOB QINCLUDE ../xdispatch/QtDispatch/Q*)

# include qt headers (if found)	
INCLUDE_DIRECTORIES(
	${QT_QTCORE_INCLUDE_DIR}
)

# moc qt files
qt4_wrap_cpp(MOC ${QHEADER} ${QINCLUDE_H})

source_group(src FILES ${QTEXT} ${QHEADER})
source_group(includes FILES ${QINCLUDE} ${QINCLUDE_H})

add_library(QtDispatch STATIC ${QTEXT} ${MOC} ${QHEADER} ${QINCLUDE} ${QINCLUDE_H})
target_link_libraries(QtDispatch xdispatch ${QT_QTCORE_LIBRARY})

mz_target_props(QtDispatch)

