@ echo off
REM #############################################################
REM
REM		Configure for Visual Studio 2010 x64
REM		(c) 2012 Marius Zwicker
REM
REM		Pass 'Release' as argument to build without debug flags
REM
REM #############################################################

@echo off

set BUILD_DIR=VS10_ProjectFiles_x64
set RELEASE_DIR=Release_%BUILD_DIR%
set GENERATOR=Visual Studio 10 Win64
set TARGET=VS10(Testing)/Windows

call "%~dp0\util.bat" %*