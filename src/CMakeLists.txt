# Copyright Institute of Sound and Vibration Research - All rights reserved

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

# Parameter message library
ADD_SUBDIRECTORY( libpml )

# Python support for calling Python functionality from C++
if( BUILD_PYTHON_BINDINGS )
  add_subdirectory( libpythonsupport )
endif( BUILD_PYTHON_BINDINGS )

# Library for signal flows.
ADD_SUBDIRECTORY( libsignalflows )

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

# Hold applications in a separate subdirectory
ADD_SUBDIRECTORY( apps )

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
  DESTINATION lib/cmake/visr
  )

# Create config files for exported targets
# for use from build tree
export(EXPORT VISR_exports
  NAMESPACE VISR::
  FILE visr_exports.cmake
  )

# Install package config file
# for use from install tree
install(FILES visr-config.cmake
  DESTINATION lib/cmake/visr)

# Copy package config file
# for use from build tree
configure_file(visr-config.cmake
  visr-config.cmake COPYONLY)

include(CMakePackageConfigHelpers)

# Create package version file
# Assuming major version bump = breaking API changes
write_basic_package_version_file(visrConfigVersion.cmake
  VERSION ${VISR_MAJOR_VERSION}.${VISR_MINOR_VERSION}.${VISR_PATCH_VERSION}
  COMPATIBILITY SameMajorVersion)

# Install package version file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/visrConfigVersion.cmake
  DESTINATION lib/cmake/visr)

# Register visr package in user registry
export(PACKAGE visr)
