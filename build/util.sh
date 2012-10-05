#!/bin/bash
##############################################################
#
#   BUILD/util.sh
#
# 	This file generates a Project Configuration for
#   building the configured Project at a default directory
#   (c) 2009-2012 Marius Zwicker
#
#   Pass 'Release' as argument to build without debug flags
#
##############################################################

function make_debug {
	cd "$BASE_DIR/$SCRIPT_DIR"
	if ! test -r $BUILD_DIR ; then
		mkdir $BUILD_DIR
	fi

	cd $BUILD_DIR

	echo "== configuring target system '$TARGET(Debug)'"
	cmake	-D CMAKE_BUILD_TYPE=Debug \
			-G"$GENERATOR" \
			$BASE_DIR/
}

function make_release {
	BUILD_DIR="$RELEASE_DIR"
	
	cd "$BASE_DIR/$SCRIPT_DIR"
	if ! test -r $BUILD_DIR ; then
		mkdir $BUILD_DIR
	fi

	cd $BUILD_DIR

	echo "== configuring target system '$TARGET(Release)'"
	cmake	-D CMAKE_BUILD_TYPE=Release \
			-G"$GENERATOR" \
			$BASE_DIR/
}

function debug_hint {
	echo
	echo -e "IMPORTANT HINT:\tWhen using this script to generate projects with build"
	echo -e "\t\ttype 'debug', please use the 'Debug' configuration for building"
	echo -e "\t\tbinaries only. Otherwise dependencies might not be set correctly."
	echo
	echo -e "\t\tTRICK:\tTo Build a Release Binary, run with argument 'Release' given"
	echo
}

function detect_dir {

	echo "== running global configuration"

	# configuration detection
	BASE_DIR=`cd "${0%/*}/.." 2>/dev/null; echo "$PWD"/"${0##*/}"`
	BASE_DIR=`dirname "$BASE_DIR"`
	
	cd "$BASE_DIR"
	if [ -d build ] 
	then
	    SCRIPT_DIR="build"
	else
	    SCRIPT_DIR="Build"
	fi
	echo "-- determining working directory: $BASE_DIR/$SCRIPT_DIR"
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
