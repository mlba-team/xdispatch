#!/bin/bash
#######################################################################
#
#  Configure Codeblocks/QtCreator project files
#  (c) 2012 Marius Zwicker
#
#  Pass 'Release' as argument to build without debug flags
#
#######################################################################

BUILD_DIR="QtCreator_ProjectFiles"
RELEASE_DIR="Release_$BUILD_DIR"
GENERATOR="CodeBlocks - Unix Makefiles"
TARGET="Qt Creator"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/util.sh $@