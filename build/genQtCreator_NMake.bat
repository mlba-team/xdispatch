@ echo off
REM #############################################################
REM
REM		Configure for CodeBlocks/QtCreator using the MSVC toolchain
REM		(c) 2012 Marius Zwicker
REM
REM		Pass 'Release' as argument to build without debug flags
REM
REM #############################################################

@echo off

set BUILD_DIR=QtCreator_NMake_ProjectFiles
set RELEASE_DIR=Release_%BUILD_DIR%
set GENERATOR=CodeBlocks - NMake Makefiles
set TARGET=QtCreator,NMake/Windows

call "%~dp0\util.bat" %*