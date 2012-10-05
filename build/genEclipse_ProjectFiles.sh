#!/bin/sh
#######################################################################
#
#  Configure Eclipse project files
#  (c) 2012 Marius Zwicker
#
#  Pass 'Release' as argument to build without debug flags
#
#######################################################################

BUILD_DIR="Eclipse_ProjectFiles"
RELEASE_DIR="Release_$BUILD_DIR"
GENERATOR="Eclipse CDT4 - Unix Makefiles"
TARGET="Eclipse CDT"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/util.sh $@