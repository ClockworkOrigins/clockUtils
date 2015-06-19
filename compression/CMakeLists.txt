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

################################
# compression cmake
################################

include_directories("include")

set(srcdir ./src)

set(compressionSrc
	${srcdir}/algorithm/HuffmanBase.cpp
	${srcdir}/algorithm/HuffmanFixed.cpp
	${srcdir}/algorithm/HuffmanGeneric.cpp
)

SOURCE_GROUP(compression FILES ${compressionSrc})
SOURCE_GROUP(algorithm FILES ${compressionSrc}/algorithm)

IF(CLOCKUTILS_BUILD_SHARED)
	add_library(clock_compression SHARED ${compressionSrc})
ELSE(CLOCKUTILS_BUILD_SHARED)
	add_library(clock_compression STATIC ${compressionSrc})
ENDIF(CLOCKUTILS_BUILD_SHARED)

IF(WIN32)
	add_custom_command(TARGET clock_compression POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/clock_compression.dll ${CMAKE_BINARY_DIR}/bin)
ENDIF(WIN32)

########################
# compression tests
########################

########## Testing ##########

IF(WITH_TESTING)
	add_subdirectory(test)
ENDIF(WITH_TESTING)

install(DIRECTORY include
	DESTINATION ${CMAKE_INSTALL_PREFIX}
	COMPONENT devel)

install(TARGETS clock_compression
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin	COMPONENT libs
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib	COMPONENT libs
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib	COMPONENT libs
)
