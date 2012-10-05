@ echo off
REM #############################################################
REM
REM		Configure for CodeBlocks/QtCreator using the MinGW toolchain
REM		(c) 2012 Marius Zwicker
REM
REM		Pass 'Release' as argument to build without debug flags
REM
REM #############################################################

@echo off

set BUILD_DIR=QtCreator_MinGW_ProjectFiles
set RELEASE_DIR=Release_%BUILD_DIR%
set GENERATOR=CodeBlocks - MinGW Makefiles
set TARGET=QtCreator,MinGW/Windows

call "%~dp0\util.bat" %*