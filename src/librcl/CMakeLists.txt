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
sparse_gain_matrix.cpp
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
sparse_gain_matrix.hpp
time_frequency_inverse_transform.hpp
time_frequency_transform.hpp
udp_receiver.hpp
udp_sender.hpp
)

add_library( rcl_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( rcl_static PROPERTIES OUTPUT_NAME rcl )
target_compile_definitions( rcl_static PUBLIC VISR_RCL_STATIC_LIBRARY=1)

add_library( rcl_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( rcl_shared PROPERTIES OUTPUT_NAME rcl )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( rcl_shared PROPERTIES ARCHIVE_OUTPUT_NAME rcl_shared )
set_target_properties( rcl_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )

# Set target properties common to both library types.
foreach(LIB_TYPE static shared)
  target_compile_definitions( rcl_${LIB_TYPE} PRIVATE VISR_BUILD_RCL_LIBRARY=1)
  # Not ideal. This is only used by certain files (scalar_osc_decoder.cpp)
  # But INCLUDE_DIRECTORIES is a source file peroperty only since Cmake 3.11
  target_include_directories( rcl_${LIB_TYPE} PRIVATE ${CMAKE_SOURCE_DIR}/3rd/include/oscpkt )
  # Transitive dependencies
  target_link_libraries( rcl_${LIB_TYPE} PUBLIC pml_${LIB_TYPE} )
  target_link_libraries( rcl_${LIB_TYPE} PUBLIC visr_${LIB_TYPE} )
  target_link_libraries( rcl_${LIB_TYPE} PUBLIC panning_${LIB_TYPE} )
  target_link_libraries( rcl_${LIB_TYPE} PUBLIC objectmodel_${LIB_TYPE} )
  target_link_libraries( rcl_${LIB_TYPE} PUBLIC rbbl_${LIB_TYPE} )
  target_link_libraries( rcl_${LIB_TYPE} PUBLIC efl_${LIB_TYPE} )
  target_link_libraries( rcl_${LIB_TYPE} PRIVATE Boost::boost )
  target_link_libraries( rcl_${LIB_TYPE} PRIVATE Boost::filesystem )
  target_link_libraries( rcl_${LIB_TYPE} PRIVATE Boost::system )
  target_link_libraries( rcl_${LIB_TYPE} PRIVATE Boost::thread )
  if( NOT Boost_USE_STATIC_LIBS )
    target_compile_definitions( rcl_${LIB_TYPE} PRIVATE -DBOOST_ALL_DYN_LINK )
  endif( NOT Boost_USE_STATIC_LIBS )
  # Apparently the Boost::disable_autolinking directive must be public even if the 
  # boost libs are used only private
  target_link_libraries( rcl_${LIB_TYPE} PUBLIC Boost::disable_autolinking )
endforeach()

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

if( BUILD_INSTALL_STATIC_LIBRARIES )
  install(TARGETS rcl_static
          EXPORT VISR_exports
          ARCHIVE DESTINATION lib COMPONENT static_libraries )
endif( BUILD_INSTALL_STATIC_LIBRARIES )
install(TARGETS rcl_shared
        EXPORT VISR_exports
        ARCHIVE DESTINATION lib COMPONENT development_files
        RUNTIME DESTINATION lib COMPONENT shared_libraries
        LIBRARY DESTINATION lib COMPONENT shared_libraries
        PUBLIC_HEADER DESTINATION include/visr/librcl COMPONENT development_files )

# Add the unit test directory.
add_subdirectory( test )
