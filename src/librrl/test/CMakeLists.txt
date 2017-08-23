# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME rrl_test )

set( SOURCES
audio_signal_flow_checking.cpp
delay_vector.cpp
parameter_connection.cpp
test_main.cpp
)

add_executable( ${APPLICATION_NAME} ${SOURCES} )

# Note: Consider moving delay_vector somewhere else (if dependency on real component libraries is not desired here.)
target_link_libraries( ${APPLICATION_NAME} signalflows )
target_link_libraries( ${APPLICATION_NAME} rcl )
target_link_libraries( ${APPLICATION_NAME} rrl )
target_link_libraries( ${APPLICATION_NAME} pml )
target_link_libraries( ${APPLICATION_NAME} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )
target_link_libraries( ${APPLICATION_NAME} ${CMAKE_THREAD_LIBS_INIT} )

set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
