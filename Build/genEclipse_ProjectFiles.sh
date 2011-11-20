#!/bin/sh
#######################################################################
#
# This file is for generating Eclipse Project Files for MS1 project
# by the use of cmake. No need to change anything here, please
# edit the corresponding CMakeLists.txt files.
# 
# (c) 2009-2010 Marius Zwicker
#
# Pass 'Release' as argument to build without debug flags
#
#######################################################################

BUILD_DIR="Eclipse_ProjectFiles"
RELEASE_DIR="Release_$BUILD_DIR"
GENERATOR="Eclipse CDT4 - Unix Makefiles"
TARGET="Eclipse CDT"

function make_debug {
	cd $BASE_DIR/Build
	if ! test -r $BUILD_DIR ; then
		mkdir $BUILD_DIR
	fi

	cd $BUILD_DIR

	echo "== configuring target system '$TARGET(Debug)'"
	cmake	-D BASE_DIR=$BASE_DIR \
			-D CMAKE_BUILD_TYPE=Debug \
			-G"$GENERATOR" \
			$BASE_DIR/
}

function make_release {
	BUILD_DIR="$RELEASE_DIR"
	
	cd $BASE_DIR/Build
	if ! test -r $BUILD_DIR ; then
		mkdir $BUILD_DIR
	fi

	cd $BUILD_DIR

	echo "== configuring target system '$TARGET(Release)'"
	cmake	-D BASE_DIR=$BASE_DIR \
			-D CMAKE_BUILD_TYPE=Release \
			-G"$GENERATOR" \
			$BASE_DIR/
}

function debug_hint {
	echo
	echo "IMPORTANT HINT:\tWhen using this script to generate projects with build"
	echo "\t\ttype 'debug', please use the 'Debug' configuration for building"
	echo "\t\tbinaries only. Otherwise dependencies might not be set correctly."
	echo
	echo "\t\tTRICK:\tTo Build a Release Binary, run with argument 'Release' given"
	echo
}

function detect_dir {

	echo "== running global configuration"

	# configuration detection
	BASE_DIR=`cd "${0%/*}" 2>/dev/null; echo "$PWD"/"${0##*/}"`
	BASE_DIR=`dirname "$BASE_DIR"`
	BASE_DIR=${BASE_DIR//\/Build/}
	echo "-- determining working directory: $BASE_DIR/build"
	echo
	
}

if [ "$1" = "Release" ]
then
	detect_dir
	make_release
else
	debug_hint
	detect_dir
	make_debug
fi

echo

exit