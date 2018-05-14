# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME rbbl_test )

add_definitions( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

set( SOURCES
 biquad_coefficient.cpp
 circular_buffer.cpp
 ffts_wrapper.cpp
 float_sequence.cpp index_sequence.cpp
 interpolating_convolver.cpp
 kiss_fft_wrapper.cpp
 parametric_iir_coefficient.cpp
 test_main.cpp
 test_FIR.cpp
 multichannel_convolver.cpp
 object_channel_allocator.cpp
 sparse_gain_routing.cpp
 test_main.cpp
)

if( BUILD_USE_IPP )
  list( APPEND SOURCES ipp_fft_wrapper.cpp)
endif( BUILD_USE_IPP )

add_executable( ${APPLICATION_NAME} ${SOURCES} )

target_link_libraries( ${APPLICATION_NAME} rbbl_static)
target_link_libraries( ${APPLICATION_NAME} pml_static)
target_link_libraries( ${APPLICATION_NAME} visr_static)
target_link_libraries( ${APPLICATION_NAME} Boost::filesystem )
target_link_libraries( ${APPLICATION_NAME} Boost::system )
target_link_libraries( ${APPLICATION_NAME} Boost::unit_test_framework )
target_link_libraries( ${APPLICATION_NAME} ${CMAKE_THREAD_LIBS_INIT} )

set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
