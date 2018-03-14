# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( APPLICATION_NAME rcl_test )

ADD_DEFINITIONS( -DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" )

ADD_EXECUTABLE( ${APPLICATION_NAME}
biquad_iir_filter.cpp
hoa_allrad_gain_calculator.cpp
scene_decoder.cpp
signal_routing.cpp
test_main.cpp
test_listener_compensation.cpp
test_kinect_receiver.cpp )

target_link_libraries( ${APPLICATION_NAME} rcl_static)
target_link_libraries( ${APPLICATION_NAME} pml_static)
target_link_libraries( ${APPLICATION_NAME} rrl_static)
target_link_libraries( ${APPLICATION_NAME} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )
target_link_libraries( ${APPLICATION_NAME} ${CMAKE_THREAD_LIBS_INIT} )

set_target_properties( ${APPLICATION_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
