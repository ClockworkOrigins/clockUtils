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

IF(ANDROID)
	ADD_DEFINITIONS(-DANDROID)
	SET(ANDROID_STL "gnustl_shared")
	SET(DEP_DIR_BUILD android)
	SET(ANDROID_SDK $ENV{ANDROID_HOME} CACHE PATH "Path to ANDROID SDK")
ELSEIF(UNIX)
	IF(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
		SET(DEP_DIR_BUILD "gcc")
		SET(UNIX_COMPILER "gcc")
	ELSEIF(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
		SET(DEP_DIR_BUILD "clang")
		SET(UNIX_COMPILER "clang")
	ENDIF()
ELSEIF(WIN32)
	IF(CMAKE_GENERATOR MATCHES "Visual Studio 12 2013 Win64")
		SET(DEP_DIR_BUILD "msvc13_64")
		SET(VS_TOOLCHAIN "msvc13")
		SET(VSENV VS12)
		SET(VS_ARCH "64")
		SET(VSSCRIPTARCH "amd64")
	ELSEIF(CMAKE_GENERATOR MATCHES "Visual Studio 12 2013")
		SET(DEP_DIR_BUILD "msvc13_32")
		SET(VS_TOOLCHAIN "msvc13")
		SET(VSENV VS12)
		SET(VS_ARCH "32")
		SET(VSSCRIPTARCH "x86")
	ELSEIF(CMAKE_GENERATOR MATCHES "Visual Studio 14 2015 Win64")
		SET(DEP_DIR_BUILD "msvc15_64")
		SET(VS_TOOLCHAIN "msvc15")
		SET(VSENV VS15)
		SET(VS_ARCH "64")
		SET(VSSCRIPTARCH "amd64")
	ELSEIF(CMAKE_GENERATOR MATCHES "Visual Studio 14 2015")
		SET(DEP_DIR_BUILD "msvc15_32")
		SET(VS_TOOLCHAIN "msvc15")
		SET(VSENV VS15)
		SET(VS_ARCH "32")
		SET(VSSCRIPTARCH "x86")
	ENDIF()
ENDIF()

SET(CLOCKUTILS_DEP_DIR "${CMAKE_SOURCE_DIR}/dependencies/${DEP_DIR_BUILD}" CACHE STRING "path to deps for clockUtils")

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Debug CACHE STRING
		"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
		FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

#########################################################################
# Global Macros and Definitions
#########################################################################

# Sets appropriate Compilerflags

SET(CXX_FLAGS "-pedantic -Wall -Wextra -Wold-style-cast -Woverloaded-virtual -Wnon-virtual-dtor -Wformat=2 -Winit-self -Wswitch-default -Wfloat-equal -Wshadow -Wredundant-decls -Wctor-dtor-privacy -Wno-sign-conversion -Wno-unused-parameter -Wno-long-long -std=c++11")
SET(CLANG_FLAGS "-Wstring-plus-int")

IF(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	SET(CMAKE_CXX_FLAGS_DEBUG		"${CXX_FLAGS} -g")
	SET(CMAKE_CXX_FLAGS_MINSIZEREL		"${CXX_FLAGS} -0s -DNDEBUG")
	SET(CMAKE_CXX_FLAGS_RELEASE		"${CXX_FLAGS} -O3 -DNDEBUG")
	SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO	"${CXX_FLAGS} -O2 -g")
	SET(CMAKE_CXX_FLAGS_MINSIZEREL		"${CMAKE_CXX_FLAGS_MINSIZEREL} -Werror")
	SET(CMAKE_CXX_FLAGS_RELEASE		"${CMAKE_CXX_FLAGS_RELEASE} -Werror")
ELSEIF(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
	SET(CMAKE_CXX_FLAGS_DEBUG		"${CXX_FLAGS} ${CLANG_FLAGS} -g")
	SET(CMAKE_CXX_FLAGS_MINSIZEREL		"${CXX_FLAGS} ${CLANG_FLAGS} -0s -DNDEBUG")
	SET(CMAKE_CXX_FLAGS_RELEASE		"${CXX_FLAGS} ${CLANG_FLAGS} -O3 -DNDEBUG")
	SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO	"${CXX_FLAGS} ${CLANG_FLAGS} -O2 -g")
	SET(CMAKE_CXX_FLAGS_MINSIZEREL		"${CMAKE_CXX_FLAGS_MINSIZEREL} -Werror")
	SET(CMAKE_CXX_FLAGS_RELEASE		"${CMAKE_CXX_FLAGS_RELEASE} -Werror")
ELSEIF(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	SET(CXX_FLAGS "/MP /W4 /wd4127 /DNOMINMAX /D_CRT_SECURE_NO_WARNINGS /D_WINSOCK_DEPRECATED_NO_WARNINGS")
	SET(CMAKE_CXX_FLAGS_DEBUG		"${CMAKE_CXX_FLAGS_DEBUG} ${CXX_FLAGS}")
	SET(CMAKE_CXX_FLAGS_MINSIZEREL		"${CMAKE_CXX_FLAGS_MINSIZEREL} ${CXX_FLAGS}")
	SET(CMAKE_CXX_FLAGS_RELEASE		"${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLAGS}")
	SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO	"${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${CXX_FLAGS}")
	SET(CMAKE_CXX_FLAGS_MINSIZEREL		"${CMAKE_CXX_FLAGS_MINSIZEREL} /WX")
	SET(CMAKE_CXX_FLAGS_RELEASE		"${CMAKE_CXX_FLAGS_RELEASE} /WX")
ELSE()
	MESSAGE(SEND_FATAL "Unknown C++ compiler \"${CMAKE_CXX_COMPILER_ID}\".")
ENDIF()

#########################################################################
# Global Directory Definitions
#########################################################################

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/" ${CMAKE_MODULE_PATH})
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
