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
option(WITH_LIBRARY_LOG "builds log library" ON)
option(WITH_LIBRARY_SOCKETS "builds sockets library" ON)
option(32BIT "builds as 32Bit library" OFF)
option(CLOCKUTILS_BUILD_SHARED "build project with shared libraries" ON)

project(clockUtils)

include(cmake/variables.cmake)

# Setting current version
SET(VERSION_MAJOR 1)
SET(VERSION_MINOR 2)
SET(VERSION_PATCH 0)
SET(DESCRIPTION "A lightweight c++ library for commonly needed tasks. Optimized for simplicity and speed.")

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin)

#########################################################################
# Library Section
#########################################################################

include(cmake/searchDependencies.cmake)

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

install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_INSTALL_PREFIX})

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

include(cmake/addToBinDir.cmake)

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
