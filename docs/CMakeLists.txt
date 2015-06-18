configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_complete_with_graphs.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_complete_with_graphs @ONLY)
add_custom_target(docu COMMAND doxygen  ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_complete_with_graphs WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
