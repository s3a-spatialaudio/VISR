# Copyright Institute of Sound and Vibration Research - All rights reserved

# Top-level build configuration for project VISceRal

# Arbitrarily set minimum version.
# This specific version is used to avoid complaints about the 
# cygwin platform not defining WIN32 anymore.
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.4)

IF( APPLE )
  SET( CMAKE_OSX_ARCHITECTURES "i386; x86_64" )
  MESSAGE( STATUS ${CMAKE_OSX_ARCHITECTURES} )
  MESSAGE( STATUS "CMAKE_OSX_SYSROOT: " ${CMAKE_OSX_SYSROOT} ) 
ENDIF( APPLE )

# The name of our top-level project.
PROJECT( VISR CXX C )

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

# Copy the executables to the binary directory
SET( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )

############################################################
# Support for Building Matlab externals

OPTION( BUILD_MATLAB_EXTERNALS "Build Matlab externals" OFF )

IF( BUILD_MATLAB_EXTERNALS )
FIND_PACKAGE(Matlab REQUIRED)
IF(MATLAB_FOUND)
    MESSAGE( STATUS "Matlab found." )
  ELSEIF( MATLAB_FOUND )
    MESSAGE( FATAL_ERROR, "Matlab not found, cancelling compilation." )
ENDIF( MATLAB_FOUND )
ENDIF( BUILD_MATLAB_EXTERNALS )

################################################################################
# Support for Max/MSP externals
IF( WIN32 OR APPLE )
  OPTION( BUILD_MAX_MSP_EXTERNALS "Build externals for MAX/MSP" OFF )
  IF( BUILD_MAX_MSP_EXTERNALS )
#    FIND_PACKAGE( MaxSDK REQUIRED HINTS ENV MAX_SDK_ROOT)
     SET( CMAKE_SYSTEM_PREFIX_PATH ${CMAKE_SYSTEM_PREFIX_PATH} ENV{MAX_SDK_ROOT} )
    FIND_PACKAGE( MaxSDK REQUIRED)
    MESSAGE( STATUS "MAX SDK found: " ${MAXSDK_MAINPATH} )
  ENDIF(BUILD_MAX_MSP_EXTERNALS)

  SET( MAXSDK_EXTERNAL_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/max )
ENDIF(  WIN32 OR APPLE )

###########################################################
# Support for 3rd-party libraries.

# Threads are required wherever we link with boost_threads.
# Note that the dependency is added automatically in most cases, so the 
# omission will go unnoticed very often.
FIND_PACKAGE(Threads REQUIRED)

## Portaudio

# For windows, we ship the includes and libraries.
# TODO: Support system-specific library directories
# TODO: Provide separate libraries for debug and release builds.
IF( WIN32 ) # This includes 64-bit platforms!
  # NOTE: It would be nicer to pass the search path to the FIND_PACKAGE
  # call instead of setting it globally.
  SET( CMAKE_SYSTEM_PREFIX_PATH ${CMAKE_SYSTEM_PREFIX_PATH} ${PROJECT_SOURCE_DIR}/3rd/portaudio )
ENDIF( WIN32 )
FIND_PACKAGE( Portaudio )
INCLUDE_DIRECTORIES( ${PORTAUDIO_INCLUDE_DIR} )

## Boost support
# On Linux and Windows, the boost libraries provided by the OS image (or brew on MacOS)
# should be found automatically.
# Otherwise (in particular for Windows platforms), an environment variable
# BOOST_ROOT should hold the path to the boost root directory.

OPTION( Boost_USE_STATIC_LIBS OFF )

# For the moment, we support only dynamic linking of boost.
# In this case, this line is required on Windows to prevent undefined symbols from boost::program_options

IF( NOT Boost_USE_STATIC_LIBS )
  ADD_DEFINITIONS("-DBOOST_ALL_DYN_LINK")
ENDIF( NOT Boost_USE_STATIC_LIBS )

# Avoid problems with the auto-link feature on Windows platforms
ADD_DEFINITIONS("-DBOOST_ALL_NO_LIB")

# Define the boost libraries that must be present.
SET( VISR_BOOST_LIBRARIES date_time filesystem program_options system thread unit_test_framework )

# Note: The minimum boost requirement is arbitrarily chosen at the moment. 
# Todo: Replace by a substantiated version requirement.
FIND_PACKAGE( Boost 1.50 REQUIRED COMPONENTS ${VISR_BOOST_LIBRARIES} )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )

################################################################################
# Actual software components (subprojects) to be built are
# contained in the src/ directory
# TODO: This is a quite hidden location for setting the subdirectories and subprojects.
# Restructure the main build file in order to make this line more prominent.
ADD_SUBDIRECTORY( src )

################################################################################
## Support for building Doxygen documentation
## Taken from: http://mementocodex.wordpress.com/2013/01/19/how-to-generate-code-documentation-with-doxygen-and-cmake-a-slightly-improved-approach/

option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ON)
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
