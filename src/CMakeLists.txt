# Copyright Institute of Sound and Vibration Research - All rights reserved

# Build configuration for the top-level src/ directory

# Common functionalties for realtime applications
ADD_SUBDIRECTORY( libapputilities )

# Elementary functions library
ADD_SUBDIRECTORY( libefl )

# Support for conversion from the IOSONO SAW scene protocol
ADD_SUBDIRECTORY( libsawconvert )

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

# Rendering infrastructure library
# Nothing to build here
ADD_SUBDIRECTORY( libril )

# Rendering runtime library 
ADD_SUBDIRECTORY( librrl )

# Data types and components related to the reverb audio object.
ADD_SUBDIRECTORY( librsao )


# Library of implementations of the interface components.
ADD_SUBDIRECTORY( libvisr_impl )

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
endif( BUILD_PYTHON_BINDINGS )
