# Copyright Institute of Sound and Vibration Research - All rights reserved

# Build configuration for the top-level src/ directory

# Elementary functions library
ADD_SUBDIRECTORY( libefl )

# Parameter message library
ADD_SUBDIRECTORY( libpml )

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
  ADD_SUBDIRECTORY( mex )
ENDIF(BUILD_MATLAB_EXTERNALS)
