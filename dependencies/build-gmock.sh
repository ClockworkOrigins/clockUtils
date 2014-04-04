#!/bin/bash

##
# Copyright 2012 FAU (Friedrich Alexander University of Erlangen-Nuremberg)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##

cd "$(readlink "$(dirname "${0}")")"

. ./build-common.sh

# googletest
ARCHIVE="gmock-1.7.0.zip"
BUILD_DIR="${BUILD_ROOT}/gmock-1.7.0"

if [ -d ${BUILD_DIR} ]; then
	rm -rf ${BUILD_DIR}
fi

PREFIX="${PWD}/gmock/"
PARALLEL_FLAG=""

if [ ! -z "${BUILD_PARALLEL}" ]; then
	PARALLEL_FLAG="-j ${BUILD_PARALLEL}"
fi

DEBUG_FLAG="-DCMAKE_OSX_ARCHITECTURES=i386;x86_64 -DCMAKE_BUILD_TYPE=Debug"
RELEASE_FLAG="-DCMAKE_OSX_ARCHITECTURES=i386;x86_64 -DCMAKE_BUILD_TYPE=Release"
if [ -z "${DEBUG}" ]; then
	BUILD_TYPE="${RELEASE_FLAG}"
else
	BUILD_TYPE="${DEBUG_FLAG}"
fi

if [ ! -z "${CLEAN}" ]; then
	status "Cleaning GoogleMock with GoogleTest"
	rm -rf "${PREFIX}"
	exit 0
fi

title "Compile GoogleMock with GoogleTest"

if ! uptodate "${EX_DIR}/${ARCHIVE}" "${PREFIX}"; then
	status "GoogleMock/Test seems to be up to date, skipping build"
	exit 0
fi

status "Extracting GoogleMock with GoogleTest"
cd "${BUILD_ROOT}"
unzip "${EX_DIR}/${ARCHIVE}" >/dev/null

status "Configuring GoogleMock with GoogleTest"
cd "${BUILD_DIR}"

cmake \
	-DCMAKE_INSTALL_PREFIX="${PREFIX}" \
	${BUILD_TYPE} \
	. >/dev/null

status "Building GoogleMock with GoogleTest"
make ${PARALLEL_FLAG} >/dev/null

status "Installing GoogleMock with GoogleTest"
mkdir -p ${PREFIX}/lib >/dev/null
cp libgmock.a ${PREFIX}/lib >/dev/null
cp gtest/libgtest.a ${PREFIX}/lib >/dev/null
cp libgmock_main.a ${PREFIX}/lib >/dev/null
cp gtest/libgtest_main.a ${PREFIX}/lib >/dev/null
cp -R include ${PREFIX} >/dev/null
cp -R gtest/include ${PREFIX} >/dev/null

status "Cleaning up"
cd "${DEP_DIR}"
rm -rf "${BUILD_DIR}" >/dev/null

touch "${PREFIX}"
