@ echo off
REM #############################################################
REM
REM		Configure for MinGW Make
REM		(c) 2012 Marius Zwicker
REM
REM		Pass 'Release' as argument to build without debug flags
REM
REM #############################################################

@echo off

set BUILD_DIR=MinGW_Makefiles
set RELEASE_DIR=Release_%BUILD_DIR%
set GENERATOR=MinGW Makefiles
set TARGET=MinGW/Windows

call "%~dp0\util.bat" %*