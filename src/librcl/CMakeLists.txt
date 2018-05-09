# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
add.cpp
biquad_iir_filter.cpp
cap_gain_calculator.cpp
channel_object_routing_calculator.cpp
crossfading_fir_filter_matrix.cpp
delay_matrix.cpp
delay_vector.cpp
diffusion_gain_calculator.cpp
fir_filter_matrix.cpp
gain_matrix.cpp
gain_vector.cpp
hoa_allrad_gain_calculator.cpp
interpolating_fir_filter_matrix.cpp
listener_compensation.cpp
null_source.cpp
object_gain_eq_calculator.cpp
panning_calculator.cpp
position_decoder.cpp
scalar_osc_decoder.cpp
scene_decoder.cpp
scene_encoder.cpp
signal_routing.cpp
time_frequency_inverse_transform.cpp
time_frequency_transform.cpp
udp_receiver.cpp
udp_sender.cpp
)

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS
add.hpp
biquad_iir_filter.hpp
cap_gain_calculator.hpp
channel_object_routing_calculator.hpp
crossfading_fir_filter_matrix.hpp
delay_matrix.hpp
delay_vector.hpp
diffusion_gain_calculator.hpp
export_symbols.hpp
fir_filter_matrix.hpp
gain_matrix.hpp
gain_vector.hpp
hoa_allrad_gain_calculator.hpp
interpolating_fir_filter_matrix.hpp
listener_compensation.hpp
null_source.hpp
object_gain_eq_calculator.hpp
panning_calculator.hpp
position_decoder.hpp
scalar_osc_decoder.hpp
scene_decoder.hpp
scene_encoder.hpp
signal_routing.hpp
time_frequency_inverse_transform.hpp
time_frequency_transform.hpp
udp_receiver.hpp
udp_sender.hpp
)

# Preliminary hack. Used only for scene_sender. 
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/3rd/include/oscpkt )


add_library( rcl_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( rcl_static PROPERTIES OUTPUT_NAME rcl )
# Transitive dependencies
target_link_libraries( rcl_static PUBLIC pml_static )
target_link_libraries( rcl_static PUBLIC visr_static )
target_link_libraries( rcl_static PUBLIC panning_static )
target_link_libraries( rcl_static PUBLIC objectmodel_static )
target_link_libraries( rcl_static PUBLIC rbbl_static )
target_link_libraries( rcl_static PUBLIC efl_static )
target_link_libraries( rcl_static PUBLIC Boost::filesystem )
target_link_libraries( rcl_static PUBLIC Boost::thread )
target_link_libraries( rcl_static PUBLIC Boost::system )

add_library( rcl_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( rcl_shared PROPERTIES OUTPUT_NAME rcl )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( rcl_shared PROPERTIES ARCHIVE_OUTPUT_NAME rcl_shared )
target_compile_definitions( rcl_shared PUBLIC -DVISR_BUILD_RCL_SHARED_LIBRARY=1)
target_compile_definitions( rcl_shared PRIVATE -DVISR_BUILD_RCL_SHARED_LIBRARY=1 -DVISR_BUILD_RCL_LIBRARY=1)
set_target_properties( rcl_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
# Transitive dependencies
target_link_libraries( rcl_shared PUBLIC pml_shared )
target_link_libraries( rcl_shared PUBLIC visr_shared )
target_link_libraries( rcl_shared PUBLIC panning_shared )
target_link_libraries( rcl_shared PUBLIC objectmodel_shared )
target_link_libraries( rcl_shared PUBLIC rbbl_shared )
target_link_libraries( rcl_shared PUBLIC efl_shared )
target_link_libraries( rcl_shared PUBLIC Boost::filesystem )
target_link_libraries( rcl_shared PUBLIC Boost::thread )
target_link_libraries( rcl_shared PUBLIC Boost::system )

# Set public headers to be installed for the shared target.
# Because the headers are the same for the static library, the headers are only
# installled once.
set_target_properties(rcl_shared PROPERTIES PUBLIC_HEADER "${HEADERS}" )

# Set include paths for dependent projects
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
foreach(TARGET_TYPE static shared)
  target_include_directories(rcl_${TARGET_TYPE}
    PUBLIC
    $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/visr>
  )
endforeach()

install(TARGETS rcl_static EXPORT VISR_exports ARCHIVE DESTINATION lib )
install(TARGETS rcl_shared EXPORT VISR_exports ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/visr/librcl )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
