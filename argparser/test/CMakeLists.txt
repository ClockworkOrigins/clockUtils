################################
# argument parser testing cmake
################################

SET(testSrc
	test_ArgumentParser.cpp
)

add_executable(ArgumentParserTester ${testSrc})

SET_TARGET_PROPERTIES(ArgumentParserTester PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(ArgumentParserTester clock_argparser ${GTEST_LIBRARIES})

IF(WIN32)
	add_custom_command(TARGET ArgumentParserTester POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/ArgumentParserTester.exe ${CMAKE_BINARY_DIR}/bin)
ENDIF(WIN32)

ADD_TEST(ArgumentParserTester ArgumentParserTester)
