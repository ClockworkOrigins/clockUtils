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

SET(CMAKE_LIBRARY_PATH ${CLOCKUTILS_DEP_DIR})

#----------------------------------------------------
# GMock
#----------------------------------------------------

IF(WITH_TESTING)
	IF(WIN32 AND NOT ANDROID AND NOT EXISTS "${CLOCKUTILS_DEP_DIR}/gmock/")
		execute_process(COMMAND ${CMAKE_SOURCE_DIR}/dependencies/build-gmock.bat ${VS_TOOLCHAIN} ${VS_ARCH} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/dependencies)
	ENDIF(WIN32 AND NOT ANDROID AND NOT EXISTS "${CLOCKUTILS_DEP_DIR}/gmock/")
	IF(UNIX AND NOT ANDROID AND NOT EXISTS "${CLOCKUTILS_DEP_DIR}/gmock/")
		execute_process(COMMAND ${CMAKE_SOURCE_DIR}/dependencies/build-gmock.sh ${UNIX_COMPILER} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/dependencies)
	ENDIF(UNIX AND NOT ANDROID AND NOT EXISTS "${CLOCKUTILS_DEP_DIR}/gmock/")
	SET(LIBNAME "GTEST")
	SET(LIBHEADER "gtest/gtest.h")
	SET(GTEST_ROOT ${CLOCKUTILS_DEP_DIR}/gmock)
	SET(GTEST_COMPONENT ${GTEST_COMPONENT} gtest)

	find_package(EasyFind REQUIRED COMPONENTS ${GTEST_COMPONENT})
	include_directories(SYSTEM ${GTEST_INCLUDE_DIR})
ENDIF(WITH_TESTING)
