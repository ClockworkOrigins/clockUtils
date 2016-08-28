#!/bin/bash

##
# clockUtils
# Copyright (2015) Michael Baer, Daniel Bonrath, All rights reserved.
#
# This file is part of clockUtils; clockUtils is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
##

set -e

# make sure this doesn't get called directly
if [ "`basename "${0}"`" == "build-common.sh" ]; then
	echo "This script should not be called directly. Please execute build-dependencies.sh instead" >&2
	exit 1
fi

# shared variables
DEP_DIR="${PWD}"
BUILD_ROOT="/tmp/`whoami`/clockUtils"
C_COMPILER=gcc
CXX_COMPILER=g++
DEP_OUT_DIR=${DEP_DIR}/gcc
CPU_CORES=`grep -c processor /proc/cpuinfo`

mkdir -p "${BUILD_ROOT}"

if [ "${1}" = "--" ] ; then
	C_COMPILER=gcc
	CXX_COMPILER=g++
	DEP_OUT_DIR=${DEP_DIR}/gcc
fi
if [ "${1}" = "gcc" ] ; then
	C_COMPILER=gcc
	CXX_COMPILER=g++
	DEP_OUT_DIR=${DEP_DIR}/gcc
fi
if [ "${1}" = "gcc-4.7" ] ; then
	C_COMPILER=gcc-4.7
	CXX_COMPILER=g++-4.7
	DEP_OUT_DIR=${DEP_DIR}/gcc-4.7
fi
if [ "${1}" = "clang" ] ; then
	C_COMPILER=clang
	CXX_COMPILER=clang++
	DEP_OUT_DIR=${DEP_DIR}/clang
fi

# print titles
title() {
	text="$1"
	echo
	echo
	echo "${text}"
	echo
	echo
}

# print status text
status() {
	text="${1}"
	echo "	${text}"
}
