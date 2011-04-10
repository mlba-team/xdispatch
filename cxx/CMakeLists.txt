
cmake_minimum_required(VERSION 2.8)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckFunctionExists)
INCLUDE (CheckSymbolExists)

# start new subproject
set(NAME xdispatch)
project(${NAME})
# core lib
file(GLOB CXX_INCL ../include/xdispatch/*.h)
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

install(TARGETS xdispatch 
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION lib)