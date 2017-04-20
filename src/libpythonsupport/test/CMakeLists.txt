# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME pythonsupport_test )

add_definitions( -DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" )

set( SOURCES
python_wrap_atom.cpp
test_main.cpp
)

add_executable( ${APPLICATION_NAME} ${SOURCES} ) 

target_link_libraries( ${APPLICATION_NAME} pythonsupport )
target_link_libraries( ${APPLICATION_NAME} rrl )
target_link_libraries( ${APPLICATION_NAME} efl )
target_link_libraries( ${APPLICATION_NAME} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )

set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
