@ echo off
REM #############################################################
REM
REM		Configure for Visual Studio 2003
REM		(c) 2012 Marius Zwicker
REM
REM		Pass 'Release' as argument to build without debug flags
REM
REM #############################################################

@echo off

set BUILD_DIR=VS_ProjectFiles
set RELEASE_DIR=Release_%BUILD_DIR%
set GENERATOR=Visual Studio 9 2008
set TARGET=VS/Windows

call "%~dp0\util.bat" %*
