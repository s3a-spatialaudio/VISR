# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
add.cpp
biquad_iir_filter.cpp
channel_object_routing_calculator.cpp
delay_matrix.cpp
delay_vector.cpp
diffusion_gain_calculator.cpp
fir_filter_matrix.cpp
gain_matrix.cpp
gain_vector.cpp
hoa_allrad_gain_calculator.cpp
listener_compensation.cpp
null_source.cpp
object_gain_eq_calculator.cpp
panning_calculator.cpp
panning_gain_calculator.cpp
position_decoder.cpp
scene_decoder.cpp
scene_encoder.cpp
signal_routing.cpp
single_to_multi_channel_diffusion.cpp
switch_decoder.cpp
time_frequency_inverse_transform.cpp
time_frequency_transform.cpp
udp_receiver.cpp
udp_sender.cpp
)

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS
add.hpp
biquad_iir_filter.hpp
channel_object_routing_calculator.hpp
delay_matrix.hpp
delay_vector.hpp
diffusion_gain_calculator.hpp
fir_filter_matrix.hpp
gain_matrix.hpp
gain_vector.cpp
hoa_allrad_gain_calculator.hpp
listener_compensation.hpp
null_source.hpp
object_gain_eq_calculator.hpp
panning_calculator.hpp
panning_gain_calculator.hpp
position_decoder.hpp
scene_decoder.hpp
scene_encoder.hpp
signal_routing.hpp
single_to_multi_channel_diffusion.hpp
switch_decoder.hpp
time_frequency_inverse_transform.hpp
time_frequency_transform.hpp
udp_receiver.hpp
udp_sender.hpp
)

# Preliminary hack. Used only for scene_sender. 
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/3rd/include/oscpkt )


ADD_LIBRARY( rcl STATIC ${SOURCES} ${HEADERS} )

# Transitive dependencies
target_link_libraries( rcl pml )
target_link_libraries( rcl ril )
target_link_libraries( rcl panning )
target_link_libraries( rcl objectmodel )
target_link_libraries( rcl rbbl )
target_link_libraries( rcl efl )

target_link_libraries( rcl ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( rcl ${Boost_THREAD_LIBRARY} )
target_link_libraries( rcl ${Boost_SYSTEM_LIBRARY} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
