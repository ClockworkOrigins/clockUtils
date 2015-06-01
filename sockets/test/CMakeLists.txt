################################
# sockets testing cmake
################################

SET(testSrc
	test_TcpSocket.cpp
	test_UdpSocket.cpp
)

add_executable(SocketsTester ${testSrc})

SET_TARGET_PROPERTIES(SocketsTester PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(SocketsTester clock_sockets ${GTEST_LIBRARIES})

IF(UNIX)
	target_link_libraries(SocketsTester pthread)
ENDIF(UNIX)

IF(WIN32)
	add_custom_command(TARGET SocketsTester POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/SocketsTester.exe ${CMAKE_BINARY_DIR}/bin)
ENDIF(WIN32)

ADD_TEST(SocketsTester SocketsTester)
