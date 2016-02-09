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
# sockets cmake
################################

set(srcdir ./src)

IF(WIN32)
	set(srcdirPlatform ./src/windows)
ELSEIF(UNIX)
	set(srcdirPlatform ./src/linux)
ENDIF()

set(socketsSrc
	${srcdir}/TcpSocket.cpp
	${srcdir}/UdpSocket.cpp
	${srcdirPlatform}/TcpSocket.cpp
	${srcdirPlatform}/UdpSocket.cpp
)

SOURCE_GROUP(sockets FILES ${socketsSrc})

IF(CLOCKUTILS_BUILD_SHARED)
	add_library(clock_sockets SHARED ${socketsSrc})
ELSE(CLOCKUTILS_BUILD_SHARED)
	add_library(clock_sockets STATIC ${socketsSrc})
ENDIF(CLOCKUTILS_BUILD_SHARED)

IF(WIN32)
	target_link_libraries(clock_sockets ws2_32)
ELSEIF(UNIX)
	# linking against pthreads or threads
	# I don't know, why we need this strange varaible, but it works
	# source: http://stackoverflow.com/questions/1620918/cmake-and-libpthread
	target_link_libraries(clock_sockets ${CMAKE_THREAD_LIBS_INIT})
ENDIF()

IF(WIN32)
	add_custom_command(TARGET clock_sockets POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/clock_sockets.dll ${CMAKE_BINARY_DIR}/bin)
ENDIF(WIN32)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/clockUtils/sockets
	DESTINATION ${CMAKE_INSTALL_PREFIX}/include/clockUtils
	COMPONENT devel)

install(TARGETS clock_sockets
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin	COMPONENT libs
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib	COMPONENT libs
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib	COMPONENT libs
)
