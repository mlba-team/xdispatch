
file(GLOB QSRC *.h *.cpp)
file(GLOB QHEADER *.h)
file(GLOB QINCLUDE_H ../include/QtDispatch/*.h)
file(GLOB QINCLUDE ../include/QtDispatch/Q*)

# include qt headers (if found)	
INCLUDE_DIRECTORIES(
	${QT_QTCORE_INCLUDE_DIR}
)

# tell qt we're building a dll
mz_add_definition(QT_MAKEDLL)

# assume an installation
include_directories(
    ../include
)

# moc qt files
mz_auto_moc(MOC ${QHEADER} ${QINCLUDE_H})

source_group(src FILES ${QSRC} ${QHEADER})
source_group(includes FILES ${QINCLUDE} ${QINCLUDE_H})

add_library(QtDispatch SHARED ${QSRC} ${MOC} ${QHEADER} ${QINCLUDE} ${QINCLUDE_H})
target_link_libraries(QtDispatch xdispatch ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

mz_target_props(QtDispatch)

install(TARGETS QtDispatch
            COMPONENT libQtDispatch
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin
            ARCHIVE DESTINATION lib)
install(DIRECTORY ../include/QtDispatch
            DESTINATION include
            COMPONENT libQtDispatch
            PATTERN "*.svn" EXCLUDE)
