# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES biquad_parameter.cpp matrix_parameter.cpp message_queue.cpp signal_routing_parameter.cpp )

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS biquad_parameter.hpp matrix_parameter.hpp message_queue.hpp signal_routing_parameter.hpp )

ADD_LIBRARY( pml STATIC ${SOURCES} ${HEADERS} )

