# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES biquad_parameter.cpp matrix_parameter.cpp message_queue.cpp )

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS biquad_parameter.hpp matrix_parameter.hpp message_queue.hpp )

ADD_LIBRARY( pml STATIC ${SOURCES} ${HEADERS} )

