# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( APPLICATION_NAME rcl_test )

ADD_DEFINITIONS( -DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" )

ADD_EXECUTABLE( ${APPLICATION_NAME} biquad_iir_filter.cpp hoa_allrad_gain_calculator.cpp scene_decoder.cpp signal_routing.cpp test_main.cpp test_listener_compensation.cpp test_kinect_receiver.cpp )

TARGET_LINK_LIBRARIES( ${APPLICATION_NAME} rcl)
TARGET_LINK_LIBRARIES( ${APPLICATION_NAME} pml)
TARGET_LINK_LIBRARIES( ${APPLICATION_NAME} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )

set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
