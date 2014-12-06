# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES matrix_parameter.cpp )

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS matrix_parameter.hpp )

ADD_LIBRARY( pml STATIC ${SOURCES} ${HEADERS} )

