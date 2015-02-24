# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES add.cpp biquad_iir_filter.cpp delay_vector.cpp gain_matrix.cpp listener_compensation.cpp null_source.cpp panning_gain_calculator.cpp position_decoder.cpp  scene_decoder.cpp signal_routing.cpp udp_receiver.cpp)

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS add.hpp biquad_iir_filter.hpp delay_vector.hpp gain_matrix.hpp listener_compensation.hpp null_source.hpp panning_gain_calculator.hpp position_decoder.hpp scene_decoder.hpp signal_routing.hpp udp_receiver.hpp)

ADD_LIBRARY( rcl STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
