# Copyright (c) 2018 Institute of Sound and Vibration Research, University of Southampton.
# All rights reserved.

find_package(Doxygen)
if(NOT DOXYGEN_FOUND)
  message(FATAL_ERROR "Doxygen is needed to build the documentation.")
endif()

set( doxyfile_in          ${PROJECT_SOURCE_DIR}/doc/doxygen/Doxyfile.in )
set( doxyfile             ${PROJECT_BINARY_DIR}/doc/doxygen/Doxyfile )
set( doxy_html_index_file ${PROJECT_BINARY_DIR}/doc/doxygen/html/index.html )
set( doxy_output_root     ${PROJECT_BINARY_DIR}/doc/doxygen/ )
set( doxy_input           ${PROJECT_SOURCE_DIR}/src )
set( doxy_extra_files     ${PROJECT_SOURCE_DIR}/doc/doxygen/ )

configure_file( ${doxyfile_in} ${doxyfile} @ONLY )

add_custom_command( OUTPUT ${doxy_html_index_file}
		  COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
		  # The following should be ${doxyfile} only but it
		  # will break the dependency.
		  # The optimal solution would be creating a 
		  # custom_command for ${doxyfile} generation
		  # but I still have to figure out how...
		  MAIN_DEPENDENCY ${doxyfile} ${doxyfile_in}
		  DEPENDS ${doxy_extra_files} # TODO <AF> Decide whether the subprojects should be added as dependency.
		  COMMENT "Generating HTML documentation")

add_custom_target( doc ALL DEPENDS ${doxy_html_index_file} )

install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html DESTINATION ${DOCUMENTATION_INSTALL_DIRECTORY} )
