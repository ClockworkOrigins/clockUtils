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

# CMake main configfile clockUtils project
#

cmake_minimum_required (VERSION 2.8)

#########################################################################
#########################################################################
#
# Global Build Configuration Options
#
#########################################################################

option(WITH_TESTING "build clockUtils with tests" OFF)
option(WITH_LIBRARY_ARGPARSER "builds argument parser library" ON)
option(WITH_LIBRARY_COMPRESSION "builds compression library" ON)
option(WITH_LIBRARY_INIPARSER "builds iniParser library" ON)
option(WITH_LIBRARY_SOCKETS "builds sockets library" ON)
option(32BIT "builds as 32Bit library" OFF)

IF(WITH_TESTING AND NOT WIN32)
	# static for gmock
	option(CLOCKUTILS_BUILD_SHARED "build project with shared libraries" OFF)
ELSE(WITH_TESTING AND NOT WIN32)
	option(CLOCKUTILS_BUILD_SHARED "build project with shared libraries" ON)
ENDIF(WITH_TESTING AND NOT WIN32)

project(clockUtils)

IF(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

#########################################################################
# Global Macros and Definitions
#########################################################################

# Sets appropriate Compilerflags

set (CXX_FLAGS "-pedantic -Wall -Wextra -Woverloaded-virtual -Wnon-virtual-dtor -Wformat=2 -Winit-self -Wswitch-default -Wfloat-equal -Wshadow -Wredundant-decls -Wctor-dtor-privacy -Wno-unused-parameter -Wno-long-long -Wno-missing-braces -std=c++11")

IF(32BIT)
	set(CXX_FLAGS "${CXX_FLAGS} -m32")
ENDIF(32BIT)

IF(WITH_TESTING)
	set(CXX_FLAGS "${CXX_FLAGS} -fPIC")
ENDIF(WITH_TESTING)
set (CLANG_FLAGS "-Wstring-plus-int")

IF(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
	set (CMAKE_CXX_FLAGS_DEBUG		"${CXX_FLAGS} ${CLANG_FLAGS} -g")
	set (CMAKE_CXX_FLAGS_MINSIZEREL		"${CXX_FLAGS} ${CLANG_FLAGS} -0s -DNDEBUG")
	set (CMAKE_CXX_FLAGS_RELEASE		"${CXX_FLAGS} ${CLANG_FLAGS} -O3 -DNDEBUG -Werror")
	set (CMAKE_CXX_FLAGS_RELWITHDEBINFO	"${CXX_FLAGS} ${CLANG_FLAGS} -O2 -g -Werror")
ELSEIF(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	set (CMAKE_CXX_FLAGS_DEBUG		"${CXX_FLAGS} -g")
	set (CMAKE_CXX_FLAGS_MINSIZEREL		"${CXX_FLAGS} -0s -DNDEBUG")
	set (CMAKE_CXX_FLAGS_RELEASE		"${CXX_FLAGS} -O3 -DNDEBUG -Werror")
	set (CMAKE_CXX_FLAGS_RELWITHDEBINFO	"${CXX_FLAGS} -O2 -g -Werror")
ELSEIF(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set (CMAKE_CXX_FLAGS_RELEASE		"${CMAKE_CXX_FLAGS_RELEASE} /MP")
	set (CMAKE_CXX_FLAGS_RELWITHDEBINFO	"${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MP")
ELSE()
	message (SEND_INFO "Unknown C++ compiler \"${CMAKE_CXX_COMPILER_ID}\". Only GNU, Clang and MSVC are completely supported.")
ENDIF()

# Setting current version
SET(VERSION_MAJOR 0)
SET(VERSION_MINOR 5)
SET(VERSION_PATCH 0)
SET(DESCRIPTION "A lightweight c++ library for commonly needed tasks. Optimized for simplicity and speed.")

#########################################################################
#Global Directory Definitions
#########################################################################

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/" ${CMAKE_MODULE_PATH})

#Setting Output Directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin)

IF(APPLE)
	set(CMAKE_INSTALL_NAME_DIR @executable_path)
	set(CMAKE_BUILD_ISIXE_INSTALL_RPATH ON)
ENDIF(APPLE)

#########################################################################
#Library Section
#########################################################################

#----------------------------------------------------
# Pthreads
#----------------------------------------------------
IF(NOT WIN32)
	find_package(Threads REQUIRED)
	include_directories(SYSTEM ${PTHREADS_INCLUDE_DIR})
ENDIF(NOT WIN32)

#----------------------------------------------------
# Google Test
#----------------------------------------------------
IF(WITH_TESTING)
	SET(LIBNAME "GTEST")
	SET(LIBHEADER "gtest/gtest.h")
	SET(GTEST_ROOT ${CMAKE_SOURCE_DIR}/dependencies/gmock)
	find_package(EasyFind REQUIRED COMPONENTS gmock_main gmock gtest)
	include_directories(SYSTEM ${GTEST_INCLUDE_DIR})
ENDIF(WITH_TESTING)

####################################################################################
# Install paths
####################################################################################

set(LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" CACHE STRING "Library directory name")
## the following are directories where stuff will be installed to
set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}" CACHE PATH "The subdirectory to the header prefix")

###############################################################################
# Debug and Testing (non permanent subdirs)
###############################################################################

IF(WITH_TESTING)
	enable_testing()
ENDIF(WITH_TESTING)

include_directories(${CMAKE_SOURCE_DIR}/include/)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
	DESTINATION ${CMAKE_INSTALL_PREFIX}
	COMPONENT devel)

configure_file(${CMAKE_SOURCE_DIR}/config/clockUtilsConfig.h.in ${CMAKE_BINARY_DIR}/include/clockUtils/Config.h @ONLY)

include_directories(${CMAKE_BINARY_DIR}/include/)

install(DIRECTORY ${CMAKE_BINARY_DIR}/include
	DESTINATION ${CMAKE_INSTALL_PREFIX}
	COMPONENT devel)

#####################################################################################
# argument parser
#####################################################################################

IF(WITH_LIBRARY_ARGPARSER)
	add_subdirectory(argParser)
	set(ARGPARSER_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/argParser/include)
ENDIF(WITH_LIBRARY_ARGPARSER)

#####################################################################################
# compression
#####################################################################################

IF(WITH_LIBRARY_COMPRESSION)
	add_subdirectory(compression)
	set(COMPRESSION_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/compression/include)
ENDIF(WITH_LIBRARY_COMPRESSION)

#####################################################################################
# iniParser
#####################################################################################

IF(WITH_LIBRARY_INIPARSER)
	add_subdirectory(iniParser)
	set(INIPARSER_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/iniParser/include)
ENDIF(WITH_LIBRARY_INIPARSER)

#####################################################################################
# sockets
#####################################################################################

IF(WITH_LIBRARY_SOCKETS)
	add_subdirectory(sockets)
	set(SOCKETS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/sockets/include)
ENDIF(WITH_LIBRARY_SOCKETS)

###############################################################################
# Docs
###############################################################################

add_subdirectory(docs)

IF(WIN32)
	file(GLOB_RECURSE files "${CMAKE_SOURCE_DIR}/dependencies/*.dll")
	foreach(filename ${files})
		execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${filename} ${CMAKE_BINARY_DIR}/bin)
	endforeach()
ENDIF(WIN32)

# set symlinks if it is an out-of-source-build
IF(NOT ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	IF(UNIX)
		execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/dependencies ${CMAKE_BINARY_DIR}/dependencies)
	ENDIF()
ENDIF(NOT ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

IF(WIN32)
	configure_file(${CMAKE_SOURCE_DIR}/config/build-releasepackages.bat.in ${CMAKE_BINARY_DIR}/build-releasepackages.bat @ONLY)
	add_custom_target(releasePackages COMMAND build-releasepackages.bat WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ELSE()
	configure_file(${CMAKE_SOURCE_DIR}/config/PKGBUILD.in ${CMAKE_BINARY_DIR}/PKGBUILD @ONLY)
ENDIF(WIN32)

SET(PACKAGE_NAME "clockutils" CACHE STRING "The name for archives and packages")

SET(CPACK_INSTALL_PREFIX "/usr")
SET(CPACK_PACKAGE_CONTACT "contact@clockwork-origins.de")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${DESCRIPTION})
SET(CPACK_PACKAGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
SET(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
SET(CPACK_COMPONENTS_ALL devel libs)
SET(CPACK_DEBIAN_PACKAGE_NAME "${PACKAGE_NAME}")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Clockwork Origins")
SET(CPACK_DEBIAN_PACKAGE_CONTACT "contact@clockwork-origins.de")
SET(CPACK_DEBIAN_PACKAGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libstdc++6 (>= 4.8.2), libgcc1 (>= 1:4.1.1), libc6 (>= 2.14)")
SET(CPACK_DEB_COMPONENT_INSTALL 1)
IF(32BIT)
	SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
ELSE(32BIT)
	SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
ENDIF(32BIT)
SET(CPACK_RPM_PACKAGE_NAME "${PACKAGE_NAME}")
SET(CPACK_RPM_PACKAGE_MAINTAINER "Clockwork Origins")
SET(CPACK_RPM_PACKAGE_CONTACT "contact@clockwork-origins.de")
SET(CPACK_RPM_PACKAGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
SET(CPACK_RPM_PACKAGE_REQUIRES "libstdc++6 (>= 4.7.2), libgcc1 (>= 1:4.1.1), libc6 (>= 2.14)")
SET(CPACK_RPM_COMPONENT_INSTALL ON)
#IF(32BIT)
#	SET(CPACK_RPM_PACKAGE_ARCHITECTURE "i686")
#ELSE(32BIT)
#	SET(CPACK_RPM_PACKAGE_ARCHITECTURE "amd64")
#ENDIF(32BIT)

INCLUDE(CPack)
