# clockUtils
# Copyright (2016) Michael Baer, Daniel Bonrath, All rights reserved.
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
# overall testing cmake
################################

SET(testSrc
	main.cpp
)

IF(WITH_LIBRARY_ARGPARSER)
	SET(testSrc
		${testSrc}
		../argParser/test_ArgumentParser.cpp
	)
ENDIF(WITH_LIBRARY_ARGPARSER)

IF(WITH_LIBRARY_COMPRESSION)
	SET(testSrc
		${testSrc}
		../compression/test_Compression.cpp
	)
ENDIF(WITH_LIBRARY_COMPRESSION)

IF(WITH_LIBRARY_CONTAINER)
	SET(testSrc
		${testSrc}
		../container/test_DoubleBufferQueue.cpp
		../container/test_LockFreeQueue.cpp
	)
ENDIF(WITH_LIBRARY_CONTAINER)

IF(WITH_LIBRARY_INIPARSER)
	SET(testSrc
		${testSrc}
		../iniParser/test_iniParser.cpp
	)
ENDIF(WITH_LIBRARY_INIPARSER)

IF(WITH_LIBRARY_LOG)
	SET(testSrc
		${testSrc}
		../log/test_log.cpp
	)
ENDIF(WITH_LIBRARY_LOG)

IF(WITH_LIBRARY_SOCKETS)
	SET(testSrc
		${testSrc}
		../sockets/test_TcpSocket.cpp
		../sockets/test_UdpSocket.cpp
	)
ENDIF(WITH_LIBRARY_SOCKETS)

add_executable(CUTester ${testSrc})

SET_TARGET_PROPERTIES(CUTester PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(CUTester ${GTEST_LIBRARIES})

IF(WITH_LIBRARY_ARGPARSER)
	target_link_libraries(CUTester clock_argParser)
ENDIF(WITH_LIBRARY_ARGPARSER)

IF(WITH_LIBRARY_COMPRESSION)
	target_link_libraries(CUTester clock_compression)
ENDIF(WITH_LIBRARY_COMPRESSION)

IF(WITH_LIBRARY_CONTAINER)
	#target_link_libraries(CUTester clock_container) # header only so far
ENDIF(WITH_LIBRARY_CONTAINER)

IF(WITH_LIBRARY_INIPARSER)
	target_link_libraries(CUTester clock_iniParser)
ENDIF(WITH_LIBRARY_INIPARSER)

IF(WITH_LIBRARY_LOG)
	target_link_libraries(CUTester clock_log)
ENDIF(WITH_LIBRARY_LOG)

IF(WITH_LIBRARY_SOCKETS)
	target_link_libraries(CUTester clock_sockets)
ENDIF(WITH_LIBRARY_SOCKETS)

IF(UNIX)
	target_link_libraries(CUTester pthread)
ENDIF(UNIX)

IF(WIN32 AND ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	add_custom_command(TARGET CUTester POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/CUTester.exe ${CMAKE_BINARY_DIR}/bin)
ENDIF(WIN32 AND ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)

ADD_TEST(CUTester CUTester)
