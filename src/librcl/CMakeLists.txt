# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES add.cpp delay_vector.cpp gain_matrix.cpp )

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS add.hpp delay_vector.hpp gain_matrix.hpp)

ADD_LIBRARY( rcl STATIC ${SOURCES} ${HEADERS} )
