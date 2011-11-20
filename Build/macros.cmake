########################################################################
#
#	BUILD/MACROS.CMAKE
#
# 	This file provides some useful macros to
#	simplify adding of componenents and other
#	taskss
#	(c) 2009-2010 Marius Zwicker
#
# This file defines a whole bunch of macros
# to add a subdirectory containing another
# CMakeLists.txt as "Subproject". All these
# Macros are not doing that much but giving
# Feedback to tell what kind of component was
# added. In all cases NAME is the name of your
# subproject and FOLDER is a relative path to
# the folder containing a CMakeLists.txt
#
# macro(mz_add_library NAME FOLDER)
#		macro for adding a new library
#
# macro(mz_add_executable NAME FOLDER)
# 		macro for adding a new executable
#
# macro(mz_add_control NAME FOLDER)
#		macro for adding a new control
#
# macro(mz_add_testtool NAME FOLDER)
#		macro for adding a folder containing testtools
#
# macro(mz_add_external NAME FOLDER)
#		macro for adding an external library/tool dependancy
#
# macro(mz_target_props NAME)
#		automatically add a "D" Postfix when compiling with Debug Symbols
#
# macro(mz_add_definition DEF)
#		add the macro DEF as definition to the compiler flags
#
# macro(mz_auto_moc MOCCED ...)
#		search all passed files in (...) for Q_OBJECT and if found
#		run moc on them via qt4_wrap_cpp. Assign the output files
#		to MOCCED
#
########################################################################

# if global.cmake was not included yet, report it
if (NOT HAS_MZ_GLOBAL)
	message(FATAL_ERROR "!! include global.cmake before including this file !!")
endif()

# no need to change anything beyond here

macro(mz_add_library NAME FOLDER)
	message("-- adding library ${NAME}")
	mz_add_target(${NAME} ${FOLDER})
endmacro(mz_add_library)

macro(mz_add_executable NAME FOLDER)
	message("-- adding executable ${NAME}")
	mz_add_target(${NAME} ${FOLDER})
endmacro(mz_add_executable)

macro(mz_add_control NAME FOLDER)
	message("-- adding control ${NAME}")
	mz_add_target(${NAME} ${FOLDER})
endmacro(mz_add_control)

macro(mz_add_testtool NAME FOLDER)
	message("-- adding testtool ${NAME}")
	mz_add_target(${NAME} ${FOLDER})
endmacro(mz_add_testtool)

macro(mz_add_external NAME FOLDER)
	message("-- adding external dependancy ${NAME}")
	mz_add_target(${NAME} ${FOLDER})
endmacro(mz_add_external)

macro(mz_add_target NAME FOLDER)
    add_subdirectory(${FOLDER} ${CMAKE_BINARY_DIR}/${NAME})
endmacro(mz_add_target)

macro(mz_target_props NAME)
    set_target_properties(${NAME} PROPERTIES DEBUG_POSTFIX "D")
endmacro()

macro(mz_extract_files _qt_files)
	set(${_qt_files})
	FOREACH(_current ${ARGN})
		file(STRINGS ${_current} _content LIMIT_COUNT 1 REGEX .*Q_OBJECT.*)
		if("${_content}" MATCHES .*Q_OBJECT.*)
			LIST(APPEND ${_qt_files} "${_current}")
		endif()
	ENDFOREACH(_current)
endmacro()

macro(mz_auto_moc mocced)
	#message("Input: ${ARGN}")
	
	set(_mocced "")
	# determine the required files
	mz_extract_files(to_moc ${ARGN})
	#message("To be mocced: ${to_moc}")
	qt4_wrap_cpp(_mocced ${to_moc})
	set(${mocced} ${${mocced}} ${_mocced})
endmacro()