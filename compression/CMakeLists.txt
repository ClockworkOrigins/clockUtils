################################
# compression cmake
################################

include_directories("include")

set(srcdir ./src)

set(compressionSrc
	${srcdir}/algorithm/HuffmanBase.cpp
	${srcdir}/algorithm/HuffmanFixed.cpp
	${srcdir}/algorithm/HuffmanGeneric.cpp
)

SOURCE_GROUP(compression FILES ${compressionSrc})
SOURCE_GROUP(algorithm FILES ${compressionSrc}/algorithm)

IF(CLOCKUTILS_BUILD_SHARED)
	add_library(clock_compression SHARED ${compressionSrc})
ELSE(CLOCKUTILS_BUILD_SHARED)
	add_library(clock_compression STATIC ${compressionSrc})
ENDIF(CLOCKUTILS_BUILD_SHARED)

IF(WIN32)
	add_custom_command(TARGET clock_compression POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/clock_compression.dll ${CMAKE_BINARY_DIR}/bin)
ENDIF(WIN32)
########################
# argument parser tests
########################

########## Testing ##########

IF(WITH_TESTING)
	add_subdirectory(test)
ENDIF(WITH_TESTING)

install(DIRECTORY include
	DESTINATION ${CMAKE_INSTALL_PREFIX}
	COMPONENT devel)

install(TARGETS clock_compression
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin	COMPONENT libs
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib	COMPONENT libs
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib	COMPONENT libs
)
