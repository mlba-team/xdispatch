#!/usr/bin/env bash

##
# Copyright (c) 2008-2014 Marius Zwicker
# All rights reserved.
# 
# @LICENSE_HEADER_START:Apache@
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# 
# http://www.mlba-team.de
# 
# @LICENSE_HEADER_END:Apache@
##

function help {
cat <<EOF

$0
==============================================

This file generates a project configuration for configuring
cmake using a predefined directory naming scheme
(c) 2009-2014 Marius Zwicker

Valid arguments:
	'help' show this list
	'mode=(release|reldbg|debug)' to control build configuration
	'compiler=(clang|gcc|vs2008|vs2010|mingw|mingw64)' to select compiler
	'generator=(ninja|makefiles|eclipse)'
	'cmake="-D.."' options to pass to cmake
	'name=".."' custom name prefix

Can also be selected using environment variables:
	MZ_CMAKETOOLS_mode=..
	MZ_CMAKETOOLS_compiler=..
	MZ_CMAKETOOLS_generator=..
	MZ_CMAKETOOLS_cmake=..
	MZ_CMAKETOOLS_name=..

EOF
}

function get_generator {
	case ${my_generator} in
		ninja)
			my_c_generator="Ninja"
			;;
		eclipse)
			my_c_generator="Eclipse CDT4 - Unix Makefiles"
			;;
		sublime)
			my_c_generator="Sublime Text 2 - Ninja"
			;;
		xcode)
			my_c_generator="Xcode"
			;;
		*)
			my_c_generator="Unix Makefiles"
			my_generator=makefiles
			;;
	esac
}

function get_mode {
	case ${my_mode} in
		release)
			my_c_mode=Release
			;;
		reldbg)
			my_c_mode=RelDbg
			;;
		*)
			my_c_mode=Debug
			my_mode=debug
			;;
	esac
}

function get_compiler {
	case ${my_compiler} in
		clang)
			my_cc=clang
			my_cxx=clang++
			;;
		*)
			my_cc=gcc
			my_cxx=g++
			my_compiler=gcc
			;;
	esac
}

function validate_config {
	if [ -z ${my_mode} ]; then
		echo "ERROR: Missing build mode"
		help
		exit
	fi

	if [ -z ${my_cc} ]; then
		echo "ERROR: Missing c compiler selection"
		help
		exit
	fi

	if [ -z ${my_cxx} ]; then
		echo "ERROR: Missing c++ compiler selection"
		help
		exit
	fi

	if [ -z ${my_generator} ]; then
		echo "ERROR: Missing cmake generator"
		help
		exit
	fi
}

function verbose {
	echo "-- my_mode=${my_mode}"
	echo "-- my_generator=${my_generator}"
	echo "-- my_c_mode=${my_c_mode}"
	echo "-- my_c_generator=${my_c_generator}"
	echo "-- my_compiler=${my_compiler}"
	echo "-- my_cxx=${my_cxx}"
	echo "-- my_cc=${my_cc}"
	echo "-- my_build_dir=${my_build_dir}"
	echo "-- my_script_dir=${my_script_dir}"
	echo "-- my_base_dir=${my_base_dir}"
	echo
}

function run_cmake {
	cd "${my_script_dir}"
	if [ ! -r ${my_build_dir} ] ; then
		mkdir -p ${my_build_dir}
	fi

	cd ${my_build_dir}

	echo "== configuring target system '${my_compiler}/${my_generator}/${my_mode}'"
    CC=${my_cc} CXX=${my_cxx} \
	cmake	-D CMAKE_BUILD_TYPE=${my_c_mode} \
			${my_cmake} \
			-G"${my_c_generator}" \
			${my_base_dir}/
}

function debug_hint {
	echo
	echo -e "IMPORTANT HINT:\tWhen using this script to generate projects with build"
	echo -e "\t\ttype 'debug', please use the 'Debug' configuration for building"
	echo -e "\t\tbinaries only. Otherwise dependencies might not be set correctly."
	echo
	echo -e "\t\tTRICK:\tTo Build a Release Binary, run with argument 'mode=release'"
	echo
}

function detect_dir {

	echo "== running global configuration"

	# configuration detection
	my_base_dir=`cd "${0%/*}/.." 2>/dev/null; echo "$PWD"/"${0##*/}"`
	my_base_dir=`dirname "${my_base_dir}"`

	my_script_dir=`cd "${0%/*}" 2>/dev/null; echo "$PWD"/"${0##*/}"`
	my_script_dir=`dirname "${my_script_dir}"`

	my_build_dir=${my_script_dir}/${my_compiler}-${my_generator}-${my_mode}
	
	echo "-- determining working directory: ${my_script_dir}"
	echo "-- build root will be: ${my_base_dir}"
	echo "-- generating to: ${my_build_dir}"
	echo
	
}

# default to using env variables
my_generator=$MZ_CMAKETOOLS_generator
my_compiler=$MZ_CMAKETOOLS_compiler
my_mode=$MZ_CMAKETOOLS_mode
my_cmake=$MZ_CMAKETOOLS_cmake

# parse the given arguments
for arg in "$@"
do
	type=`echo ${arg} | awk -F "=" '{print $1}'`
	value=`echo ${arg} | awk -F "=" '{print $2}'`
	
	case ${type} in
		generator)
			my_generator=${value}
			;;
		mode)
			my_mode=${value}
			;;
		compiler)
			my_compiler=${value}
			;;
		cmake)
			my_cmake=${value}
			;;
		help)
			help
			exit 0
			;;
		*)
			echo "-- WARNING: Unknown flag ${type}, skipping"
			;;
	esac
done

# convert arguments to params
get_compiler
get_mode
get_generator

# fallback to gcc
if [ -z ${my_cc} ]; then
	my_cc=gcc
fi

if [ -z ${my_cxx} ]; then
	my_cxx=g++
fi

# get the working directory
detect_dir

# print obtained variable values
#verbose

if [ "${my_mode}" = "debug" ]; then
	debug_hint
fi

# finally execute the cmake generation
validate_config
run_cmake

echo

exit
