# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES array_configuration.cpp  biquad_parameter.cpp empty_parameter_config.cpp filter_routing_parameter.cpp float_sequence.cpp index_sequence.cpp listener_position.cpp matrix_parameter.cpp matrix_parameter_config.cpp message_queue.cpp signal_routing_parameter.cpp  string_parameter.cpp string_parameter_config.cpp)

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS array_configuration.hpp biquad_parameter.hpp empty_parameter_config.hpp filter_routing_parameter.hpp float_sequence.hpp index_sequence.hpp  listener_position.hpp matrix_parameter.hpp matrix_parameter_config.hpp message_queue.hpp signal_routing_parameter.hpp string_parameter.hpp string_parameter_config.hpp)

INCLUDE_DIRECTORIES( ${SNDFILE_INCLUDE_DIRS} )

ADD_LIBRARY( pml STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
