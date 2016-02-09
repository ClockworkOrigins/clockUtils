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
# compression testing cmake
################################

SET(testSrc
	test_Compression.cpp
)

add_executable(CompressionTester ${testSrc})

SET_TARGET_PROPERTIES(CompressionTester PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(CompressionTester clock_compression ${GTEST_LIBRARIES})

IF(UNIX)
	TARGET_LINK_LIBRARIES(CompressionTester pthread)
ENDIF(UNIX)

IF(WIN32)
	add_custom_command(TARGET CompressionTester POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/CompressionTester.exe ${CMAKE_BINARY_DIR}/bin)
ENDIF(WIN32)

ADD_TEST(CompressionTester CompressionTester)
