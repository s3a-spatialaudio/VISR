# Copyright Institute of Sound and Vibration Research - All rights reserved

# Set the installation directory for the export and version files
if( VISR_SYSTEM_NAME MATCHES "Linux" )
  # On  Linux, there's a predefined directory structure under /usr/lib
  set( VISR_INSTALL_EXPORT_CONFIG_DIRECTORY lib/cmake/visr )
else( VISR_SYSTEM_NAME MATCHES "Linux" )
  # On Windows and MacOS, install it in a cmake/ directory within the application's install directory,
  # e.g. c:\Program Files\VISR-X.X.X/cmake or /Applications/VISR-X.X.X/cmake
  set( VISR_INSTALL_EXPORT_CONFIG_DIRECTORY cmake )
endif( VISR_SYSTEM_NAME MATCHES "Linux" )

# Build configuration for the top-level src/ directory

# Common functionalties for realtime applications
ADD_SUBDIRECTORY( libapputilities )

# Audio interfaces library
ADD_SUBDIRECTORY( libaudiointerfaces )

# Elementary functions library
ADD_SUBDIRECTORY( libefl )

# Object model library.
ADD_SUBDIRECTORY( libobjectmodel )

# Panning library, compute a panning gains for a loudspeaker configuration
ADD_SUBDIRECTORY( libpanning )

# Signal processing components and data types related to panning.
add_subdirectory( libpanningdsp )

# Parameter message library
ADD_SUBDIRECTORY( libpml )

# Python support for calling Python functionality from C++
if( BUILD_PYTHON_BINDINGS )
  add_subdirectory( libpythonsupport )
endif( BUILD_PYTHON_BINDINGS )

# Library for signal flows.
ADD_SUBDIRECTORY( libsignalflows )

# Library for signal flows that use Python.
# Temporary solution to avoid a link dependency on Python
# for the majority of signal flows.
# Note that this library is also build if BUILD_PYTHON_BINDINGS is unselected.
# In this case, it contains a reduced functionality that does not use Python.
add_subdirectory( libsignalflowspython )


# Rendering building block library
ADD_SUBDIRECTORY( librbbl )

# Rendering component library
ADD_SUBDIRECTORY( librcl )

# Rendering runtime library
ADD_SUBDIRECTORY( librrl )

# Data types and components related to the reverb audio object.
ADD_SUBDIRECTORY( libreverbobject )

# VISR main library
ADD_SUBDIRECTORY( libvisr )

if( BUILD_STANDALONE_APPLICATIONS )
  # Hold applications in a separate subdirectory
  add_subdirectory( apps )
endif( BUILD_STANDALONE_APPLICATIONS )

# Subdirectory for Matlab Mex externals
IF( BUILD_MATLAB_EXTERNALS )
  # Library containing general support classes and functions for Matlab Mex externals
  ADD_SUBDIRECTORY( libmexsupport )

  # Directory containing the Mex externals itself
  ADD_SUBDIRECTORY( mex )
ENDIF(BUILD_MATLAB_EXTERNALS)

IF( BUILD_MAX_MSP_EXTERNALS )
  ADD_SUBDIRECTORY( maxmspexternals )
ENDIF( BUILD_MAX_MSP_EXTERNALS )

# Python bindings for the rendering framework
if( BUILD_PYTHON_BINDINGS )
  add_subdirectory( python )
  add_subdirectory( libpythoncomponents )
endif( BUILD_PYTHON_BINDINGS )

##### CMake packaging

# Install exported targets as config files
# for use from install tree
install(EXPORT VISR_exports
  NAMESPACE VISR::
  FILE visr_exports.cmake
  DESTINATION ${VISR_INSTALL_EXPORT_CONFIG_DIRECTORY}
  COMPONENT development_files
  )

# Create config files for exported targets
# for use from build tree
export(EXPORT VISR_exports
  NAMESPACE VISR::
  FILE visr_exports.cmake
  )

# Install package config file
# for use from install tree
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/visr-config.cmake
  DESTINATION ${VISR_INSTALL_EXPORT_CONFIG_DIRECTORY} COMPONENT development_files )

# Copy package config file
# for use from build tree, substituting Boost setting variables
configure_file(visr-config.cmake
  visr-config.cmake @ONLY)

include(CMakePackageConfigHelpers)

# Create package version file
# Assuming major version bump = breaking API changes
write_basic_package_version_file(visr-config-version.cmake
  VERSION ${VISR_MAJOR_VERSION}.${VISR_MINOR_VERSION}.${VISR_PATCH_VERSION}
  COMPATIBILITY SameMajorVersion)

# Install package version file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/visr-config-version.cmake
  DESTINATION ${VISR_INSTALL_EXPORT_CONFIG_DIRECTORY} COMPONENT development_files )

# Register visr package in user registry
export(PACKAGE visr)
