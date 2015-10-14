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
# container testing cmake
################################

SET(testSrc
	test_DoubleBufferQueue.cpp
)

add_executable(ContainerTester ${testSrc})

SET_TARGET_PROPERTIES(ContainerTester PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(ContainerTester ${GTEST_LIBRARIES})

IF(UNIX)
	target_link_libraries(ContainerTester pthread)
ENDIF(UNIX)

IF(WIN32)
	add_custom_command(TARGET ContainerTester POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/ContainerTester.exe ${CMAKE_BINARY_DIR}/bin)
ENDIF(WIN32)

ADD_TEST(ContainerTester ContainerTester)
