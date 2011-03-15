########################################################################
#
#	BUILD/QTMACROS.CMAKE
#
# 	This file provides some useful macros to
#	add moc support and link against needed qt
# 	libraries by using simple commands.
#	(c) 2009-2010 Marius Zwicker
#
#
# macro(mz_qtproject NAME)
#		call this to add a new qaxserver project to your build tree
#		where NAME is your project name
#
# macro(mz_qtautomoc MOCCEDFILES HFILES)
# 		use this macro to moc your files using
# 		Q_OBJECT macro. MOCCEDFILES will contain a
# 		list of the new generated moc_*.cpp files, HFILES
# 		must be your input headerfiles.
#
########################################################################

# if global.cmake was not included yet, report it
if (NOT HAS_MZ_GLOBAL)
	message(FATAL_ERROR "!! include global.cmake before including this file !!")
endif()

# no need to change anything beyond here
# detect current qt4 installation
find_package(Qt4 COMPONENTS QtCore QtTest)

macro(mz_qtproject NAME)
	project(${NAME})
	include(${QT_USE_FILE})
endmacro(mz_qtproject NAME)

macro(mz_qtautomoc MOCCEDFILES HFILES)
	qt4_wrap_cpp(${MOCCEDFILES} ${HFILES})
endmacro(mz_qtautomoc)
