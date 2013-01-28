#
# Copyright (c) 2011-2013 MLBA-Team. All rights reserved.
#
# @MLBA_OPEN_LICENSE_HEADER_START@
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# @MLBA_OPEN_LICENSE_HEADER_END@
#

file(GLOB QSRC *.h *.cpp)
file(GLOB QHEADER *.h)
file(GLOB QINCLUDE_H
    ../include/QtDispatch/*.h
    ../include/QtDispatch/QBlockRunnable
    ../include/QtDispatch/QDispatch
    ../include/QtDispatch/QDispatchApplication
    ../include/QtDispatch/QDispatchCoreApplication
    ../include/QtDispatch/QDispatchGroup
    ../include/QtDispatch/QDispatchQueue
    ../include/QtDispatch/QDispatchSemaphore
    ../include/QtDispatch/QDispatchSource
    ../include/QtDispatch/QDispatchTimer
    ../include/QtDispatch/QIterationBlockRunnable
    ../include/QtDispatch/QIterationRunnable
    ../include/QtDispatch/QtDispatch
)
file(GLOB QINCLUDE ../include/QtDispatch/Q*)

# include qt headers (if found)	
INCLUDE_DIRECTORIES(
	${QT_QTCORE_INCLUDE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

# tell qt we're building a dll
mz_add_definition(QT_MAKEDLL)

# assume an installation
include_directories(
    ../include
)

# moc qt files
mz_auto_moc(MOC ${QHEADER} ${QINCLUDE_H})
if(XDISPATCH_USE_QT5)
    qt5_generate_moc(qdispatchgroup.cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_qdispatchgroup.moc)
else()
    qt4_generate_moc(qdispatchgroup.cpp ${CMAKE_CURRENT_BINARY_DIR}/moc_qdispatchgroup.moc)
endif()
set(QMOC ${CMAKE_CURRENT_BINARY_DIR}/moc_qdispatchgroup.moc)

source_group(src FILES ${QSRC} ${QHEADER})
source_group(includes FILES ${QINCLUDE} ${QINCLUDE_H})

# res file
if(WIN32)
	set(RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
endif()

if( IOS )
	set( QTDISPATCH_BUILD_TYPE STATIC )
else()
	set( QTDISPATCH_BUILD_TYPE SHARED )
endif()

add_library(QtDispatch ${QTDISPATCH_BUILD_TYPE} ${QSRC} ${MOC} ${QHEADER} ${QINCLUDE} ${QMOC} ${QINCLUDE_H} ${RES_FILE})
set_property(TARGET QtDispatch PROPERTY SOVERSION ${XDISPATCH_VERSION})
if(XDISPATCH_USE_QT5)
    target_link_libraries(QtDispatch xdispatch)
    qt5_use_modules(QtDispatch Widgets Network)
else()
    target_link_libraries(QtDispatch xdispatch ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
endif()

mz_target_props(QtDispatch)
set_property(TARGET QtDispatch PROPERTY VERSION ${XDISPATCH_VERSION})

if(IOS)
    install(TARGETS QtDispatch
                LIBRARY DESTINATION lib
                RUNTIME DESTINATION bin
                ARCHIVE DESTINATION lib)
    install(DIRECTORY ../include/QtDispatch
                DESTINATION include
                PATTERN "*.svn" EXCLUDE)
                
elseif(APPLE)
    set_property(TARGET QtDispatch PROPERTY FRAMEWORK TRUE)
    set_property(TARGET QtDispatch PROPERTY PUBLIC_HEADER ${QINCLUDE_H})

    install(TARGETS QtDispatch
                COMPONENT libQtDispatch
                FRAMEWORK DESTINATION "Library/Frameworks")
else()
    install(TARGETS QtDispatch
                COMPONENT libQtDispatch
                LIBRARY DESTINATION lib
                RUNTIME DESTINATION bin
                ARCHIVE DESTINATION lib)
    install(DIRECTORY ../include/QtDispatch
                DESTINATION include
                COMPONENT libQtDispatch-dev
                PATTERN "*.svn" EXCLUDE)
endif()
