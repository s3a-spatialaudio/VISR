# Copyright Institute of Sound and Vibration Research - All rights reserved

# Top-level build configuration for project VISceRal

# Arbitrarily set minimum version.
# This specific version is used to avoid complaints about the 
# cygwin platform not defining WIN32 anymore.
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.4)

# The name of our top-level project.
PROJECT( VISR CXX )

# Add support for unit tests
INCLUDE( CTest )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")

# TBD: Switch between GNU compiler and clang on Linux platforms
OPTION( SET_CLANG_COMPILER "Use the clang compiler on Linux platforms" OFF )

# Enable C++11 features on Linux platforms
# See http://www.cmake.org/Wiki/CMake/Tutorials/C%2B%2B11Flags
# Maybe there is a more general way to configure the different platforms and toolchains
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
endif()

# At the moment, all include directories are relative to the source root directory.
# Therefore it suffices to set this root as the include path.
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src )

# Include files of imported (i.e., 3rd party) libraries must be specified relative to 3rd/include.
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/3rd/include )

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake_modules)

############################################################
# Support for Building Matlab externals

OPTION( BUILD_MATLAB_EXTERNALS "Build Matlab externals" ON )

IF( BUILD_MATLAB_EXTERNALS )
FIND_PACKAGE(Matlab REQUIRED)
IF(MATLAB_FOUND)
    MESSAGE( STATUS "Matlab found." )
  ELSEIF( MATLAB_FOUND )
    MESSAGE( FATAL_ERROR, "Matlab not found, cancelling compilation." )
ENDIF( MATLAB_FOUND )
ENDIF( BUILD_MATLAB_EXTERNALS )

###########################################################
# Define imported libraries

if(WIN32) # This includes 64-bit platforms!
  add_library(portaudio SHARED IMPORTED)
  set_property(TARGET portaudio PROPERTY IMPORTED_LOCATION_DEBUG ${PROJECT_SOURCE_DIR}/3rd/lib/windows_x86_64_msvc13/portaudio/debug/portaudio_x64.dll)
  set_property(TARGET portaudio PROPERTY IMPORTED_LOCATION_RELEASE ${PROJECT_SOURCE_DIR}/3rd/lib/windows_x86_64_msvc13/portaudio/release/portaudio_x64.dll)
  set_property(TARGET portaudio PROPERTY IMPORTED_IMPLIB_DEBUG ${PROJECT_SOURCE_DIR}/3rd/lib/windows_x86_64_msvc13/portaudio/debug/portaudio_x64.lib)
  set_property(TARGET portaudio PROPERTY IMPORTED_IMPLIB_RELEASE ${PROJECT_SOURCE_DIR}/3rd/lib/windows_x86_64_msvc13/portaudio/release/portaudio_x64.lib)
else(WIN32)
  find_package( Portaudio )
endif( WIN32 )

## Boost support

# Define the boost libraries that must be present.
SET( VISR_BOOST_LIBRARIES unit_test_framework )

# Note: The minimum boost requirement is arbitrarily chosen at the moment. 
# Todo: Replace by a substantiated version requirement.
FIND_PACKAGE( Boost 1.50 REQUIRED COMPONENTS system thread unit_test_framework)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )

# Actual software components (subprojects) to be built are
# contained in the src/ directory
ADD_SUBDIRECTORY( src )

################################################################################
## Support for building Doxygen documentation
## Taken from: http://mementocodex.wordpress.com/2013/01/19/how-to-generate-code-documentation-with-doxygen-and-cmake-a-slightly-improved-approach/

option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" OFF)
IF(BUILD_DOCUMENTATION) 
  FIND_PACKAGE(Doxygen)
  IF(NOT DOXYGEN_FOUND)
    MESSAGE(FATAL_ERROR
      "Doxygen is needed to build the documentation.")
  ENDIF()
 
  SET( doxyfile_in          ${PROJECT_SOURCE_DIR}/doc/src/Doxyfile.in )
  SET( doxyfile             ${PROJECT_SOURCE_DIR}/doc/Doxyfile )
  SET( doxy_html_index_file ${PROJECT_SOURCE_DIR}/doc/html/index.html )
  SET( doxy_output_root     ${PROJECT_SOURCE_DIR}/doc ) # Pasted into Doxyfile.in
  SET( doxy_input           ${PROJECT_SOURCE_DIR}/src                   ) # Pasted into Doxyfile.in
#  SET( doxy_extra_files     ${PROJECT_SOURCE_DIR}/doc/src/mainpage.dox    ) # Pasted into Doxyfile.in
 
  CONFIGURE_FILE( ${doxyfile_in} ${doxyfile} @ONLY )
 
  ADD_CUSTOM_COMMAND( OUTPUT ${doxy_html_index_file}
                      COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                      # The following should be ${doxyfile} only but it
                      # will break the dependency.
                      # The optimal solution would be creating a 
                      # custom_command for ${doxyfile} generation
                      # but I still have to figure out how...
                      MAIN_DEPENDENCY ${doxyfile} ${doxyfile_in}
                      DEPENDS ${doxy_extra_files} # TODO <AF> Decide whether the subprojects should be added as dependency.
                      COMMENT "Generating HTML documentation")
 
  ADD_CUSTOM_TARGET( doc ALL DEPENDS ${doxy_html_index_file} )
 
  INSTALL( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc )
ENDIF()
