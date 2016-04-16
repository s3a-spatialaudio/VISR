# Copyright Institute of Sound and Vibration Research - All rights reserved

# Build configuration for the top-level src/ directory

# Common functionalties for realtime applications built
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

# Library for signal flows.
ADD_SUBDIRECTORY( libsignalflows )

# Rendering building block library
ADD_SUBDIRECTORY( librbbl )

# Rendering component library
ADD_SUBDIRECTORY( librcl )

# Rendering infrastructure library 
ADD_SUBDIRECTORY( libril )

# Rendering runtime library 
ADD_SUBDIRECTORY( librrl )

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

IF( BUILD_PYTHON_BINDINGS )
  ADD_SUBDIRECTORY( pythonbindings )
ENDIF( BUILD_PYTHON_BINDINGS )
