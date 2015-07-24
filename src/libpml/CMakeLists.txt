# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES array_configuration.cpp  biquad_parameter.cpp index_list.cpp listener_position.cpp matrix_parameter.cpp message_queue.cpp signal_routing_parameter.cpp )

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS array_configuration.hpp biquad_parameter.hpp index_list.hpp  listener_position.hpp matrix_parameter.hpp message_queue.hpp signal_routing_parameter.hpp )

ADD_LIBRARY( pml STATIC ${SOURCES} ${HEADERS} )

