# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME pythoncomponents_test )

set( SOURCES
test_main.cpp
wrapper.cpp
)

add_executable( ${APPLICATION_NAME} ${SOURCES} ) 

target_link_libraries( ${APPLICATION_NAME} pythoncomponents_shared )
target_link_libraries( ${APPLICATION_NAME} rrl_shared )
target_link_libraries( ${APPLICATION_NAME} Boost::unit_test_framework )
target_link_libraries( ${APPLICATION_NAME} ${CMAKE_THREAD_LIBS_INIT} )

# Since the 'wrapper' unit tests require the visr Python external, ensure that it is compiled
add_dependencies( ${APPLICATION_NAME} visrpython )

# Encode the current source location to give access to the Python code contained in the test/python directory.
target_compile_definitions( ${APPLICATION_NAME} PRIVATE CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

# Encode the path to the visr Python external into a compile definition
target_compile_definitions( ${APPLICATION_NAME} PRIVATE PYTHON_MODULE_INSTALL_DIRECTORY="$<TARGET_FILE_DIR:visrpython>" )

set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
