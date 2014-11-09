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
