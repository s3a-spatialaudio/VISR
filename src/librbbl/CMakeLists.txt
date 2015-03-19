# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES fir.cpp gain_matrix.cpp )

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS fir.hpp gain_matrix.hpp )

ADD_LIBRARY( rbbl STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
