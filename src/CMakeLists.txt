# Copyright Institute of Sound and Vibration Research - All rights reserved

# Top-level build configuration for project VISceRal

# Arbitrarily set minimum version.
# This specific version is used to avoid complaints about the cygwin platform not
# defining WIN32 anymore.
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.4)

# The name of our top-level project.
PROJECT( VISR CXX )

# TBD: Switch between GNU compiler and clang on Linux platforms
OPTION( SET_CLANG_COMPILER "Use the clang compiler on Linux platforms" OFF )

# Enable C++11 features on Linux platforms
# See http://www.cmake.org/Wiki/CMake/Tutorials/C%2B%2B11Flags
# Maybe there is a more general way to configure the different platforms and toolchains
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
endif()

SET( CMAKE_CXX_FLAGS, "${PROJECT_SOURCE_DIR}" )


MESSAGE( STATUS "FLAGS= ${CMAKE_CXX_FLAGS}" )
MESSAGE( STATUS "PATH= ${CMAKE_MODULE_PATH}" )
MESSAGE( STATUS "dir: ${CMAKE_SOURCE_DIR}" )

# FIND_LIBRARY( portaudio REQUIRED )


# At the moment, all include directories are relative to the source root directory.
# Therefore it suffices to set this root as the include path.
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR} )

# Elementary functions library
ADD_SUBDIRECTORY( libefl )

# Rendering infrastructure library 
ADD_SUBDIRECTORY( libril )

# Rendering runtime library 
ADD_SUBDIRECTORY( librrl )

# Rendering component library
ADD_SUBDIRECTORY( librcl )

# Hold applications in a separate subdirectory
ADD_SUBDIRECTORY( apps )
