################################
# iniParsers testing cmake
################################

SET(testSrc
	test_iniParser.cpp
)

add_executable(iniParserTester ${testSrc})

SET_TARGET_PROPERTIES(iniParserTester PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(iniParserTester clock_iniParser ${GTEST_LIBRARIES})

IF(UNIX)
	target_link_libraries(iniParserTester pthread)
ENDIF(UNIX)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR}/bin)

IF(WIN32)
	add_custom_command(TARGET iniParserTester POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/iniParserTester.exe ${CMAKE_BINARY_DIR}/bin)
ENDIF(WIN32)

ADD_TEST(iniParserTester iniParserTester)
