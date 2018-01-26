# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
add.cpp
biquad_iir_filter.cpp
channel_object_routing_calculator.cpp
delay_vector.cpp
diffusion_gain_calculator.cpp
fir_filter_matrix.cpp
gain_matrix.cpp
hoa_allrad_gain_calculator.cpp
listener_compensation.cpp
null_source.cpp
object_gain_eq_calculator.cpp
panning_gain_calculator.cpp
CAP_gain_calculator.cpp
position_decoder.cpp
scene_decoder.cpp
scene_encoder.cpp
signal_routing.cpp
single_to_multi_channel_diffusion.cpp
udp_receiver.cpp
udp_sender.cpp
)

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS
add.hpp
biquad_iir_filter.hpp
channel_object_routing_calculator.hpp
delay_vector.hpp
diffusion_gain_calculator.hpp
fir_filter_matrix.hpp
gain_matrix.hpp
hoa_allrad_gain_calculator.hpp
listener_compensation.hpp
null_source.hpp
object_gain_eq_calculator.hpp
panning_gain_calculator.hpp
CAP_gain_calculator.hpp
position_decoder.hpp
scene_decoder.hpp
scene_encoder.hpp
signal_routing.hpp
single_to_multi_channel_diffusion.hpp
udp_receiver.hpp
udp_sender.hpp
)

ADD_LIBRARY( rcl STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
