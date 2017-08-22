# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME visr_impl_test )

set( SOURCES
audio_connection_descriptor.cpp
audio_sample_type.cpp
component_names.cpp
parameter_factory.cpp
parameter_connection.cpp
polymorphic_parameter_port.cpp
test_main.cpp
)

add_executable( ${APPLICATION_NAME} ${SOURCES} )

target_link_libraries( ${APPLICATION_NAME} visr_static)
target_link_libraries( ${APPLICATION_NAME} ril)
target_link_libraries( ${APPLICATION_NAME} pml )
target_link_libraries( ${APPLICATION_NAME} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )
target_link_libraries( ${APPLICATION_NAME} ${CMAKE_THREAD_LIBS_INIT} )

set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
