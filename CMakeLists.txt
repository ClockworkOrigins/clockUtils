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
option(WITH_LIBRARY_CONTAINER "builds container library" ON)
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

set (CXX_FLAGS "-pedantic -Wall -Wextra -Woverloaded-virtual -Wnon-virtual-dtor -Wformat=2 -Winit-self -Wswitch-default -Wfloat-equal -Wshadow -Wredundant-decls -Wctor-dtor-privacy -Wno-unused-parameter -Wno-long-long -Wno-missing-braces -std=c++11 ")

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
	set (CXX_FLAGS "/MP /W4 /wd4127")
	set (CMAKE_CXX_FLAGS_DEBUG		"${CMAKE_CXX_FLAGS_DEBUG} ${CXX_FLAGS}")
	set (CMAKE_CXX_FLAGS_MINSIZEREL	"${CMAKE_CXX_FLAGS_MINSIZEREL} ${CXX_FLAGS}")
	set (CMAKE_CXX_FLAGS_RELEASE		"${CMAKE_CXX_FLAGS_RELEASE} ${CXX_FLAGS}")
	set (CMAKE_CXX_FLAGS_RELWITHDEBINFO	"${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${CXX_FLAGS}")
ELSE()
	message (SEND_INFO "Unknown C++ compiler \"${CMAKE_CXX_COMPILER_ID}\". Only GNU, Clang and MSVC are completely supported.")
ENDIF()

# Setting current version
SET(VERSION_MAJOR 1)
SET(VERSION_MINOR 0)
SET(VERSION_PATCH 1)
SET(DESCRIPTION "A lightweight c++ library for commonly needed tasks. Optimized for simplicity and speed.")

#########################################################################
# Global Directory Definitions
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
# Dependency Path extensions
#########################################################################

IF(WIN32)
	IF(CMAKE_GENERATOR MATCHES "Visual Studio 12")
		SET(DEP_DIR_BUILD "msvc12")
	ELSEIF(CMAKE_GENERATOR MATCHES "Visual Studio 14")
		SET(DEP_DIR_BUILD "msvc14")
	ELSE(CMAKE_GENERATOR MATCHES "Win64")
		MESSAGE([WARNING] "Toolchain not supported yet!")
	ENDIF()

	IF(CMAKE_GENERATOR MATCHES "Win64")
		SET(DEP_DIR_BUILD "${DEP_DIR_BUILD}_x64")
	ELSEIF(CMAKE_GENERATOR MATCHES "ARM")
		SET(DEP_DIR_BUILD "${DEP_DIR_BUILD}_arm")
	ELSE()
		SET(DEP_DIR_BUILD "${DEP_DIR_BUILD}_x86")
	ENDIF()
	SET(COMPILER_VERSION ${DEP_DIR_BUILD})
ENDIF(WIN32)

#########################################################################
# Library Section
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
	SET(GTEST_ROOT ${CMAKE_SOURCE_DIR}/dependencies/${DEP_DIR_BUILD}/gmock)
	find_package(EasyFind REQUIRED COMPONENTS gmock_main gmock gtest)
	include_directories(SYSTEM ${GTEST_INCLUDE_DIR})
ENDIF(WITH_TESTING)

####################################################################################
# Install paths
####################################################################################

set(LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" CACHE STRING "Library directory name")
## the following are directories where stuff will be installed to
set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}" CACHE PATH "The subdirectory to the header prefix")

include_directories(${CMAKE_SOURCE_DIR}/include/)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
	DESTINATION ${CMAKE_INSTALL_PREFIX}
	COMPONENT devel)

configure_file(${CMAKE_SOURCE_DIR}/config/clockUtilsConfig.h.in ${CMAKE_BINARY_DIR}/include/clockUtils/Config.h @ONLY)

include_directories(${CMAKE_BINARY_DIR}/include/)

install(DIRECTORY ${CMAKE_BINARY_DIR}/include
	DESTINATION ${CMAKE_INSTALL_PREFIX}
	COMPONENT devel)

add_subdirectory(libs)

###############################################################################
# Debug and Testing (non permanent subdirs)
###############################################################################

IF(WITH_TESTING)
	add_subdirectory(tests)
ENDIF(WITH_TESTING)

###############################################################################
# Docs
###############################################################################

add_subdirectory(docs)

IF(WIN32)
	file(GLOB_RECURSE files "${CMAKE_SOURCE_DIR}/dependencies/${DEP_DIR_BUILD}/*.dll")
	foreach(filename ${files})
		execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${filename} ${CMAKE_BINARY_DIR}/bin)
	endforeach()
ENDIF(WIN32)

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
SET(CPACK_DEBIAN_devel_PACKAGE_DEPENDS "libc6-dev (>= 2.14), clockutils-libs (= ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})")
SET(CPACK_DEBIAN_libs_PACKAGE_DEPENDS "libstdc++6 (>= 4.8.2), libgcc1 (>= 1:4.1.1), libc6 (>= 2.14)")
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
SET(CPACK_RPM_PACKAGE_REQUIRES "libstdc++6 >= 4.7.2, libgcc_s1 >= 2.2.4, glibc >= 2.14")
SET(CPACK_RPM_devel_PACKAGE_REQUIRES "glibc-devel >= 2.14, clockutils-libs >= 0.5.0, libclock_argParser.so, libclock_compression.so, libclock_iniParser.so, libclock_sockets.so")
SET(CPACK_RPM_libs_PACKAGE_PROVIDES "libclock_argParser.so, libclock_compression.so, libclock_iniParser.so, libclock_sockets.so")
SET(CPACK_RPM_COMPONENT_INSTALL ON)
#IF(32BIT)
#	SET(CPACK_RPM_PACKAGE_ARCHITECTURE "i686")
#ELSE(32BIT)
#	SET(CPACK_RPM_PACKAGE_ARCHITECTURE "amd64")
#ENDIF(32BIT)

INCLUDE(CPack)
