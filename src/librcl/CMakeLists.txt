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
scalar_osc_decoder.cpp
scene_decoder.cpp
scene_encoder.cpp
signal_routing.cpp
single_to_multi_channel_diffusion.cpp
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
export_symbols.hpp
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
scalar_osc_decoder.hpp
scene_decoder.hpp
scene_encoder.hpp
signal_routing.hpp
single_to_multi_channel_diffusion.hpp
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

add_library( rcl_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( rcl_shared PROPERTIES OUTPUT_NAME rcl )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( rcl_shared PROPERTIES ARCHIVE_OUTPUT_NAME rcl_shared )
target_compile_definitions( rcl_shared PRIVATE -DVISR_BUILD_RCL_SHARED_LIBRARY=1 -DVISR_BUILD_RCL_LIBRARY=1)
set_target_properties( rcl_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
# Transitive dependencies
target_link_libraries( rcl_shared pml_shared )
target_link_libraries( rcl_shared visr_impl_shared )
target_link_libraries( rcl_shared panning )
target_link_libraries( rcl_shared objectmodel_shared )
target_link_libraries( rcl_shared rbbl )
target_link_libraries( rcl_shared efl )
target_link_libraries( rcl_shared ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( rcl_shared ${Boost_THREAD_LIBRARY} )
target_link_libraries( rcl_shared ${Boost_SYSTEM_LIBRARY} )


# Add the unit test directory.
ADD_SUBDIRECTORY( test )
