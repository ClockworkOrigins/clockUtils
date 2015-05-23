################################
# argParser cmake
################################

include_directories("include")

set(srcdir ./src)

set(argParserSrc
	${srcdir}/BasicVariable.cpp
	${srcdir}/Parser.cpp
)

SOURCE_GROUP(argParser FILES ${argParserSrc})

IF(CLOCKUTILS_BUILD_SHARED)
	add_library(clock_argParser SHARED ${argParserSrc})
ELSE(CLOCKUTILS_BUILD_SHARED)
	add_library(clock_argParser STATIC ${argParserSrc})
ENDIF(CLOCKUTILS_BUILD_SHARED)

IF(WIN32)
	add_custom_command(TARGET clock_argParser POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/clock_argParser.dll ${CMAKE_BINARY_DIR}/bin)
ENDIF(WIN32)
########################
# argument parser tests
########################

########## Testing ##########

IF(WITH_TESTING)
	add_subdirectory(test)
ENDIF(WITH_TESTING)

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})

install(TARGETS clock_argParser
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
