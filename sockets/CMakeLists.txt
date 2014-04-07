################################
# sockets cmake
################################

include_directories("include")

set(srcdir ./src)

IF(WIN32)
	set(srcdirPlatform ./src/windows)
ELSEIF(UNIX)
	set(srcdirPlatform ./src/linux)
ENDIF()

set(socketsSrc
	${srcdir}/TcpSocket.cpp
	${srcdirPlatform}/TcpSocket.cpp
)

SOURCE_GROUP(sockets FILES ${socketsSrc})

IF(CLOCKUTILS_BUILD_SHARED)
	add_library(clock_sockets SHARED ${socketsSrc})
ELSE(CLOCKUTILS_BUILD_SHARED)
	add_library(clock_sockets STATIC ${socketsSrc})
ENDIF(CLOCKUTILS_BUILD_SHARED)

IF(WIN32)
	target_link_libraries(clock_sockets ws2_32)
ELSEIF(UNIX)
	target_link_libraries(clock_sockets pthread)
ENDIF()

IF(WIN32)
	add_custom_command(TARGET clock_sockets POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/clock_sockets.dll ${CMAKE_BINARY_DIR}/bin)
ENDIF(WIN32)
########################
# i6engine-utils tests
########################

########## Testing ##########

IF(WITH_TESTING)
	add_subdirectory(test)
ENDIF(WITH_TESTING)

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})

install(TARGETS clock_sockets
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
