# Copyright Institute of Sound and Vibration Research - All rights reserved

# Top-level build configuration for project VISceRal

# Arbitrarily set minimum version.
# This specific version is used to avoid complaints about the 
# cygwin platform not defining WIN32 anymore.
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.4)

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
# INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/3rd/include )

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake_modules)

# Copy the executables to the binary directory
SET( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )

#############################################################
# Create a system string.
SET( VISR_SYSTEM_NAME ${CMAKE_SYSTEM_NAME} )
IF( VISR_SYSTEM_NAME MATCHES "Darwin" )
  SET( VISR_SYSTEM_NAME MacOS )
ENDIF( VISR_SYSTEM_NAME MATCHES "Darwin" )

SET( VISR_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR} )
IF( VISR_SYSTEM_PROCESSOR MATCHES "AMD64" )
  SET( VISR_SYSTEM_PROCESSOR x86_64 )
ENDIF( VISR_SYSTEM_PROCESSOR MATCHES "AMD64" )
STRING( TOLOWER ${VISR_SYSTEM_NAME}-${VISR_SYSTEM_PROCESSOR} VISR_PLATFORM_NAME )
# FOr use as a C/C++ preprocessor definition, we must not use hyphens.
STRING( TOLOWER ${VISR_SYSTEM_NAME}_${VISR_SYSTEM_PROCESSOR} VISR_PLATFORM_NAME_MACRO )


MESSAGE( STATUS "VISR system processor: " ${VISR_SYSTEM_PROCESSOR} )
MESSAGE( STATUS "VISR system name:      " ${VISR_SYSTEM_NAME} )
MESSAGE( STATUS "VISR platform name:    " ${VISR_PLATFORM_NAME} )

ADD_DEFINITIONS("-DVISR_SYSTEM_PROCESSOR_${VISR_SYSTEM_PROCESSOR}")
ADD_DEFINITIONS("-DVISR_SYSTEM_NAME_${VISR_SYSTEM_NAME}")
ADD_DEFINITIONS("-DVISR_PLATFORM_NAME_${VISR_PLATFORM_NAME_MACRO}")

############################################################
# Create universal binaries on MacOS
IF( VISR_SYSTEM_NAME MATCHES "MacOS" )
  OPTION( BUILD_MACOS_UNIVERSAL_BINARIES "Universal (32/64-bit) binaries on MacOS X" ON)
  IF( BUILD_MACOS_UNIVERSAL_BINARIES )
    SET( OSX_UNIVERSAL ON )
    SET( CMAKE_OSX_ARCHITECTURES "i386;x86_64" )
  ENDIF( BUILD_MACOS_UNIVERSAL_BINARIES )
ENDIF( VISR_SYSTEM_NAME MATCHES "MacOS" )

############################################################
# Set default visibility of shared library symbols to hidden (as on Windows platforms)
# TODO: Test and fix this fature.
SET( CMAKE_C_VISIBILITY_PRESET hidden )
SET( CMAKE_CXX_VISIBILITY_PRESET hidden )

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
    SET( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{MAX_SDK_ROOT} )
    FIND_PACKAGE( MaxSDK REQUIRED)
    MESSAGE( STATUS "MAX SDK found: " ${MAXSDK_MAINPATH} )
  ENDIF(BUILD_MAX_MSP_EXTERNALS)

  SET( MAXSDK_EXTERNAL_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/max )
ENDIF(  WIN32 OR APPLE )

################################################################################
# Support for Python bindings of selected VISR functionality
OPTION( BUILD_PYTHON_BINDINGS "Python bindings of selected VISR functionality" OFF )

IF( BUILD_PYTHON_BINDINGS )
  FIND_PACKAGE( PythonLibs REQUIRED )
  # Brute-force way make all the code position-independent if we use python bindings.
  # TODO: Devise more selective solution (separate sets of static libraries, etc.)
  SET( CMAKE_POSITION_INDEPENDENT_CODE True )
ENDIF( BUILD_PYTHON_BINDINGS )

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
  LIST( APPEND CMAKE_INCLUDE_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/include/portaudio) )
  LIST( APPEND CMAKE_LIBRARY_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/lib/${VISR_PLATFORM_NAME}/portaudio) )
 
ENDIF( WIN32 )
FIND_PACKAGE( Portaudio )
INCLUDE_DIRECTORIES( ${PORTAUDIO_INCLUDE_DIR} )

## Jack Audio Connection Kit (JACK)
OPTION( BUILD_USE_JACK "Support the Jack (Jack Audio Connection Kit) audio interface" OFF )
IF( BUILD_USE_JACK )
  # Preliminary support for testing the Jack library on Windows platforms.
  IF( WIN32 )
    SET( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{JACK_ROOT} )
  ENDIF( WIN32 )
  FIND_PACKAGE( Jack REQUIRED)
ENDIF( BUILD_USE_JACK )

## Libsndfile
# On Linux and MacOS, we use the versions provided by the OS (or brew on MacOS)
IF( WIN32 ) # This includes 64-bit platforms!
  # NOTE: It would be nicer to pass the search path to the FIND_PACKAGE
  # call instead of setting it globally.
  LIST( APPEND CMAKE_INCLUDE_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/include/sndfile) )
  LIST( APPEND CMAKE_LIBRARY_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/lib/${VISR_PLATFORM_NAME}/sndfile) )
ENDIF( WIN32 )
FIND_PACKAGE( SndFile REQUIRED )

## KissFFT: Basic FFT support (portable, slow and with a permissive licence)
# This libbrary is provided by 3rd/ for all platforms, because it is no standard library (moreover, heavily tweaked to geet both float and double support)
  LIST( APPEND CMAKE_INCLUDE_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/include/kissfft) )
  LIST( APPEND CMAKE_LIBRARY_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/lib/${VISR_PLATFORM_NAME}/kissfft) )
FIND_PACKAGE( KissFFT REQUIRED )

## FFTW
OPTION( BUILD_USE_FFTW "Use the FFTW library. Do not use for distribution, because this makes the whole project GPL." OFF )
IF( BUILD_USE_FFTW )
  IF( WIN32 ) # This includes 64-bit platforms.
    LIST( APPEND CMAKE_PREFIX_PATH $ENV{FFTW_ROOT} )
  ENDIF( WIN32 )
  FIND_PACKAGE( FFTW REQUIRED )
ENDIF( BUILD_USE_FFTW )

# IPP
OPTION( BUILD_USE_IPP "Use the Intel IPP for fast arithmetic functions and trasnforms. Set the environment variable IPP_ROOT accordingly." OFF )
IF( BUILD_USE_IPP )
  # Use the variable IPP_ROOT to point to the base directory of the library.
  FIND_PACKAGE( IPP REQUIRED )
ENDIF( BUILD_USE_IPP )

## FFTS (Fastest Fourier Transform in the south)
# Because this is a non-standard library, we provide it for all platforms.
# TODO: Consider making this library optional
LIST( APPEND CMAKE_INCLUDE_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/include/ffts) )
LIST( APPEND CMAKE_LIBRARY_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/lib/${VISR_PLATFORM_NAME}/ffts) )
FIND_PACKAGE( FFTS REQUIRED )

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
# In boost 1.59 on Windows, chrono is an implicit dependency of boost_thread. So in order to locate the runtime DLL for packaging, we mus add this library here.
SET( VISR_BOOST_LIBRARIES chrono date_time filesystem program_options system thread unit_test_framework )

IF( BUILD_PYTHON_BINDINGS )
  LIST( APPEND VISR_BOOST_LIBRARIES python ) 
ENDIF( BUILD_PYTHON_BINDINGS )

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
  SET( doxyfile             ${PROJECT_BINARY_DIR}/doc/Doxyfile )
  SET( doxy_html_index_file ${PROJECT_BINARY_DIR}/doc/html/index.html )
  SET( doxy_output_root     ${PROJECT_BINARY_DIR}/doc ) # Pasted into Doxyfile.in
  SET( doxy_input           ${PROJECT_SOURCE_DIR}/src ) # Pasted into Doxyfile.in
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
 
  INSTALL( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html DESTINATION share/doc )
ENDIF()

# Package generation
# TODO: Move this stuff to an external location
# TODO: Add support for other systems (Windows installer, MacOS images)

SET( CPACK_DEBIAN_PACKAGE_MAINTAINER "Andreas Franck A.Franck@soton.ac.uk" )
SET( CPACK_PACKAGE_NAME "VISR" )
SET( CPACK_PACKAGE_VENDOR "ISVR")

IF( WIN32 )
SET( CPACK_GENERATOR "NSIS" )
SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
GET_FILENAME_COMPONENT( PORTAUDIO_LIBRARY_DIR ${PORTAUDIO_LIBRARY} DIRECTORY )
INSTALL( FILES ${PORTAUDIO_LIBRARY_DIR}/portaudio_x64.dll DESTINATION bin )
IF(BUILD_USE_FFTW)
  GET_FILENAME_COMPONENT( FFTW_LIBRARY_DIR ${FFTW3_LIBRARY} DIRECTORY )
  INSTALL( FILES ${FFTW_LIBRARY_DIR}/libfftw3-3.dll DESTINATION bin )
  INSTALL( FILES ${FFTW_LIBRARY_DIR}/libfftw3f-3.dll DESTINATION bin )
ENDIF(BUILD_USE_FFTW)
# Boost
IF( NOT Boost_USE_STATIC_LIBS )
  GET_FILENAME_COMPONENT( BOOST_CHRONO_LIBRARY_BASENAME ${Boost_CHRONO_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_DATE_TIME_LIBRARY_BASENAME ${Boost_DATE_TIME_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_FILESYSTEM_LIBRARY_BASENAME ${Boost_FILESYSTEM_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_PROGRAM_OPTIONS_LIBRARY_BASENAME ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_SYSTEM_LIBRARY_BASENAME ${Boost_SYSTEM_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_THREAD_LIBRARY_BASENAME ${Boost_THREAD_LIBRARY_RELEASE} NAME_WE )
  INSTALL( FILES ${Boost_LIBRARY_DIR}/${BOOST_CHRONO_LIBRARY_BASENAME}.dll DESTINATION bin )
  INSTALL( FILES ${Boost_LIBRARY_DIR}/${BOOST_DATE_TIME_LIBRARY_BASENAME}.dll DESTINATION bin )
  INSTALL( FILES ${Boost_LIBRARY_DIR}/${BOOST_FILESYSTEM_LIBRARY_BASENAME}.dll DESTINATION bin )
  INSTALL( FILES ${Boost_LIBRARY_DIR}/${BOOST_PROGRAM_OPTIONS_LIBRARY_BASENAME}.dll DESTINATION bin )
  INSTALL( FILES ${Boost_LIBRARY_DIR}/${BOOST_SYSTEM_LIBRARY_BASENAME}.dll DESTINATION bin )
  INSTALL( FILES ${Boost_LIBRARY_DIR}/${BOOST_THREAD_LIBRARY_BASENAME}.dll DESTINATION bin )
ENDIF( NOT Boost_USE_STATIC_LIBS )
ENDIF( WIN32 )

IF( VISR_SYSTEM_NAME MATCHES "Linux" )
  SET( CPACK_GENERATOR "DEB" )
  SET( CPACK_DEBIAN_HOMEPAGE "http://www.s3a-spatialaudio.org" )
  SET( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
ENDIF(VISR_SYSTEM_NAME MATCHES "Linux")

IF( VISR_SYSTEM_NAME MATCHES "MacOS" )
  SET( CPACK_GENERATOR "Bundle" )
  SET( CPACK_BUNDLE_NAME "VISR" )
  IF( NOT Boost_USE_STATIC_LIBS )
    INSTALL( FILES ${PORTAUDIO_LIBRARIES} DESTINATION bin)
    INSTALL( FILES ${Boost_FILESYSTEM_LIBRARY} DESTINATION bin )
    INSTALL( FILES ${Boost_SYSTEM_LIBRARY} DESTINATION bin )
    INSTALL( FILES ${Boost_THREAD_LIBRARY} DESTINATION bin )
    INSTALL( FILES ${Boost_PROGRAM_OPTIONS_LIBRARY} DESTINATION bin )
    INSTALL( FILES ${CMAKE_THREAD_LIBS_INIT} DESTINATION bin )
  ENDIF( NOT Boost_USE_STATIC_LIBS )
ENDIF( VISR_SYSTEM_NAME MATCHES "MacOS" )

INSTALL( DIRECTORY config DESTINATION share/visr )
INSTALL( FILES ${CMAKE_SOURCE_DIR}/licence.txt DESTINATION share/visr )
SET( CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/licence.txt )

INCLUDE( CPack )
