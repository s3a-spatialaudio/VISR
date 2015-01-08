# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES matrix_parameter.cpp message_queue.cpp )

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS matrix_parameter.hpp message_queue.hpp )

ADD_LIBRARY( pml STATIC ${SOURCES} ${HEADERS} )

