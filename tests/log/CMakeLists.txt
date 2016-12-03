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
# Logs testing cmake
################################

SET(testSrc
	main.cpp

	test_log.cpp
)

add_executable(LogTester ${testSrc})

SET_TARGET_PROPERTIES(LogTester PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(LogTester clock_log ${GTEST_LIBRARIES})

IF(UNIX)
	target_link_libraries(LogTester pthread)
ENDIF(UNIX)

IF(WIN32 AND ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	add_custom_command(TARGET LogTester POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/LogTester.exe ${CMAKE_BINARY_DIR}/bin)
ENDIF(WIN32 AND ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)

ADD_TEST(LogTester LogTester)
