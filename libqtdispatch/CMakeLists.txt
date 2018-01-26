#
# CMakeLists.txt
#
# Copyright (c) 2011-2018 MLBA-Team
# All rights reserved.
#
# @LICENSE_HEADER_START@
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
# @LICENSE_HEADER_END@
#

file(GLOB QSRC src/*.h src/*.cpp)
file(GLOB QHEADER src/*.h)
file(GLOB QINCLUDE_H
    include/QtDispatch/*.h
    include/QtDispatch/QBlockRunnable
    include/QtDispatch/QDispatch
    include/QtDispatch/QDispatchApplication
    include/QtDispatch/QDispatchCoreApplication
    include/QtDispatch/QDispatchGroup
    include/QtDispatch/QDispatchQueue
    include/QtDispatch/QDispatchSemaphore
    include/QtDispatch/QDispatchSource
    include/QtDispatch/QDispatchTimer
    include/QtDispatch/QIterationBlockRunnable
    include/QtDispatch/QIterationRunnable
    include/QtDispatch/QtDispatch
)
file(GLOB QINCLUDE include/QtDispatch/Q*)

# tell qt we're building a dll
mz_add_definition(QT_MAKEDLL)

# moc qt files
set( CMAKE_AUTOMOC ON )

source_group(src FILES ${QSRC} ${QHEADER})
source_group(includes FILES ${QINCLUDE} ${QINCLUDE_H})

# res file
if(MZ_WINDOWS)
        set(RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
endif()

if( MZ_IOS )
    set( QTDISPATCH_BUILD_TYPE STATIC )
else()
    set( QTDISPATCH_BUILD_TYPE SHARED )
endif()

add_library(QtDispatch ${QTDISPATCH_BUILD_TYPE} ${QSRC} ${MOC} ${QHEADER} ${QINCLUDE} ${QINCLUDE_H} ${RES_FILE})
# include qt headers (if found)
target_include_directories(QtDispatch
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
    INTERFACE include
)
target_link_libraries(QtDispatch xdispatch)
if( NOT MZ_MACOS )
    target_link_libraries(QtDispatch dispatch)
endif()
qt5_use_modules(QtDispatch Widgets Network)

set_target_properties( QtDispatch PROPERTIES
    VERSION ${XDISPATCH_VERSION}
    SOVERSION ${XDISPATCH_VERSION}
)

if(MZ_IOS)
    install(TARGETS QtDispatch
                LIBRARY DESTINATION lib
                RUNTIME DESTINATION bin
                ARCHIVE DESTINATION lib)
    install(DIRECTORY include/QtDispatch
                DESTINATION include
                PATTERN "*.svn" EXCLUDE)

elseif(MZ_APPLE)
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
    install(DIRECTORY include/QtDispatch
                DESTINATION include
                COMPONENT libQtDispatch-dev
                PATTERN "*.svn" EXCLUDE)
endif()

mz_target_props(QtDispatch)
mz_auto_format(QtDispatch)
