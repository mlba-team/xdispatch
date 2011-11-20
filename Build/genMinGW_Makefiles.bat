@ echo off
REM #############################################################
REM
REM		BUILD/genVS6_ProjectFiles.bat
REM
REM 	This file generates a MinGW Makefile configuration for
REM		building configured Project
REM		(c) 2009-2011 Marius Zwicker
REM
REM		Pass 'Release' as argument to build without debug flags
REM
REM #############################################################

@echo off

set BUILD_DIR=MinGW_Makefiles
set RELEASE_DIR=Release_%BUILD_DIR%
set GENERATOR=MinGW Makefiles
set TARGET=MinGW/Windows

goto MAIN

:make_debug
	cd %BASE_DIR%\Build
	if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
	cd %BUILD_DIR%

	echo.== configuring target system '%TARGET%(Debug)'
	cmake	-D BASE_DIR=%BASE_DIR% -D CMAKE_BUILD_TYPE=Debug -G"%GENERATOR%" %BASE_DIR%/
GOTO:EOF

:make_release
	set BUILD_DIR="%RELEASE_DIR%"
	
	cd %BASE_DIR%\Build
	if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
	cd %BUILD_DIR%

	echo.== configuring target system '%TARGET%(Release)'
	cmake	-D BASE_DIR=%BASE_DIR% -D CMAKE_BUILD_TYPE=Release -G"%GENERATOR%" %BASE_DIR%/
GOTO:EOF

:debug_hint
	echo.
	echo.IMPORTANT HINT: When using this script to generate projects with build
	echo.type 'debug', please use the 'Debug' configuration for building
	echo.binaries only. Otherwise dependencies might not be set correctly.
	echo.
	echo.TRICK: To Build a Release Binary, run with argument 'Release' given
GOTO:EOF

:detect_dir

	echo.
	echo.== running global configuration

	REM dirty hack, um root pfad korrekt herauszufinden
	cd ..
	set BASE_DIR=%CD%
	cd Build
	echo.-- determining working directory: %BASE_DIR%\Build
	echo.
	
GOTO:EOF

:MAIN
if "%1%" == "Release" goto CALL_RELEASE

call:debug_hint
call:detect_dir
call:make_debug
goto END

:CALL_RELEASE
call:detect_dir
call:make_release
goto END

:END
cd ..
echo.All DONE
pause
