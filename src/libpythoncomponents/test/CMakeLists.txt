# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME pythoncomponents_test )

add_definitions( -DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" )

set( SOURCES
test_main.cpp
wrapper.cpp
)

add_executable( ${APPLICATION_NAME} ${SOURCES} ) 

target_link_libraries( ${APPLICATION_NAME} pythoncomponents_shared )
target_link_libraries( ${APPLICATION_NAME} rrl_shared )
target_link_libraries( ${APPLICATION_NAME} Boost::unit_test_framework )
target_link_libraries( ${APPLICATION_NAME} ${CMAKE_THREAD_LIBS_INIT} )

set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
