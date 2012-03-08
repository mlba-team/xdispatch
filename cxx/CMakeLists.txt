
cmake_minimum_required(VERSION 2.8)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckFunctionExists)
INCLUDE (CheckSymbolExists)

# start new subproject
set(NAME xdispatch)
project(${NAME})

# core lib
file(GLOB CXX_INCL ../include/xdispatch/*.h ../include/xdispatch/dispatch)
file(GLOB CXX *.cpp *.h)
source_group(xdispatch FILES ${CXX_INCL})
source_group(cxx FILES ${CXX})
  
# add new target xdispatch
mz_add_definition(XDISPATCH_MAKEDLL)
add_library(${NAME} SHARED ${CXX_INCL} ${CXX})
  
# link this target with all needed libraries
message("-- linking xdispatch with: ${LIBS}")
target_link_libraries(${NAME} ${LIBS})
mz_target_props(${NAME})

set_property(TARGET xdispatch PROPERTY VERSION ${XDISPATCH_VERSION})
set_property(TARGET xdispatch PROPERTY SOVERSION ${XDISPATCH_VERSION})

if(APPLE)
    set_property(TARGET xdispatch PROPERTY FRAMEWORK TRUE)
    set_property(TARGET xdispatch PROPERTY PUBLIC_HEADER ${CXX_INCL})

    install(TARGETS xdispatch
                COMPONENT libxdispatch
                FRAMEWORK DESTINATION "Library/Frameworks")
else()
    install(TARGETS xdispatch
                COMPONENT libxdispatch
                LIBRARY DESTINATION lib
                RUNTIME DESTINATION bin
                ARCHIVE DESTINATION lib)
    install(DIRECTORY ../include/xdispatch
                DESTINATION include
                COMPONENT libxdispatch-dev
                PATTERN "*.svn" EXCLUDE)
endif()

