# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES gain_matrix.cpp )

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS gain_matrix.hpp )

ADD_LIBRARY( rbbl STATIC ${SOURCES} ${HEADERS} )
