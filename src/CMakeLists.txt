# Copyright Institute of Sound and Vibration Research - All rights reserved

# Build configuration for the top-level src/ directory

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
