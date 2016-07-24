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
# iniParsers testing cmake
################################

SET(testSrc
	main.cpp

	test_iniParser.cpp
)

add_executable(iniParserTester ${testSrc})

SET_TARGET_PROPERTIES(iniParserTester PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(iniParserTester clock_iniParser ${GTEST_LIBRARIES})

IF(UNIX)
	target_link_libraries(iniParserTester pthread)
ENDIF(UNIX)

add_custom_command(TARGET iniParserTester PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/bin/resources)

IF(WIN32 AND ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	add_custom_command(TARGET iniParserTester POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/iniParserTester.exe ${CMAKE_BINARY_DIR}/bin)
ENDIF(WIN32 AND ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)

ADD_TEST(iniParserTester iniParserTester)
