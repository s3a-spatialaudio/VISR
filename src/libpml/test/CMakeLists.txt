# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME pml_test )

add_definitions( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

set( SOURCES
 filter_routing_parameter.cpp
 matrix_parameter.cpp
 parameter_instantiation.cpp
 test_main.cpp
)

add_executable( ${APPLICATION_NAME} ${SOURCES} )

target_link_libraries( ${APPLICATION_NAME} pml )
target_link_libraries( ${APPLICATION_NAME} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )
target_link_libraries( ${APPLICATION_NAME} ${CMAKE_THREAD_LIBS_INIT} )

set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
