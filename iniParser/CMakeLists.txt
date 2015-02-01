################################
# iniParser cmake
################################

include_directories("include")

set(srcdir ./src)

set(iniParserSrc
	${srcdir}/iniParser.cpp
)

SOURCE_GROUP(iniParser FILES ${iniParserSrc})

IF(CLOCKUTILS_BUILD_SHARED)
	add_library(clock_iniParser SHARED ${iniParserSrc})
ELSE(CLOCKUTILS_BUILD_SHARED)
	add_library(clock_iniParser STATIC ${iniParserSrc})
ENDIF(CLOCKUTILS_BUILD_SHARED)

IF(WIN32)
	add_custom_command(TARGET clock_iniParser POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/clock_iniParser.dll ${CMAKE_BINARY_DIR}/bin)
ENDIF(WIN32)
########################
# i6engine-utils tests
########################

########## Testing ##########

IF(WITH_TESTING)
	add_subdirectory(test)
ENDIF(WITH_TESTING)

install(DIRECTORY include
	DESTINATION ${CMAKE_INSTALL_PREFIX}
	COMPONENT devel)

install(TARGETS clock_iniParser
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin	COMPONENT libs
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib	COMPONENT libs
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib	COMPONENT libs
)
