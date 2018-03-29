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

cd "$(readlink "$(dirname "${0}")")"

. ./build-common.sh ${1}

ARCHIVE="gmock-1.7.0.zip"
BUILD_DIR="${BUILD_ROOT}/gmock-1.7.0"
PREFIX="${DEP_OUT_DIR}/gmock/"

if [ -d "${PREFIX}" ]; then
	exit 0
fi

title "Compile GoogleMock with GoogleTest"

status "Extracting GoogleMock with GoogleTest"

downloadAndUnpack ${ARCHIVE}

status "Configuring GoogleMock with GoogleTest"

cd "${BUILD_DIR}"
cmake \
	-DCMAKE_INSTALL_PREFIX="${PREFIX}" \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_C_COMPILER=${C_COMPILER} \
	-DCMAKE_CXX_COMPILER=${CXX_COMPILER} \
.

status "Building GoogleMock with GoogleTest"

make -j ${CPU_CORES}

status "Installing GoogleMock with GoogleTest"

mkdir -p "${PREFIX}/lib"
cp libgmock.a "${PREFIX}/lib"
cp gtest/libgtest.a "${PREFIX}/lib"
cp libgmock_main.a "${PREFIX}/lib"
cp gtest/libgtest_main.a "${PREFIX}/lib"
cp -R include "${PREFIX}"
cp -R gtest/include "${PREFIX}"

status "Cleaning up"

cd "${DEP_DIR}"
rm -rf "${BUILD_ROOT}"

