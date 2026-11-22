# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME pythoncomponents_test )

set( SOURCES
pml_initialisation.cpp
test_main.cpp
wrapper.cpp
)

set( HEADERS
run_python_adder.hpp
)

add_executable( ${APPLICATION_NAME} ${SOURCES} ${HEADERS} )

target_link_libraries( ${APPLICATION_NAME} PRIVATE pythoncomponents_shared )
target_link_libraries( ${APPLICATION_NAME} PRIVATE pythonsupport_shared )
target_link_libraries( ${APPLICATION_NAME} PRIVATE rrl_shared )
target_link_libraries( ${APPLICATION_NAME} PRIVATE pml_shared )
target_link_libraries( ${APPLICATION_NAME} PRIVATE Boost::filesystem )
target_link_libraries( ${APPLICATION_NAME} PRIVATE Boost::unit_test_framework )

if( NOT Boost_USE_STATIC_LIBS )
  target_compile_definitions(${APPLICATION_NAME} PRIVATE -DBOOST_ALL_DYN_LINK )
endif( NOT Boost_USE_STATIC_LIBS )
target_compile_definitions( ${APPLICATION_NAME} PRIVATE -DBOOST_ALL_NO_LIB )

# The Python components used in the tests import the visr and pml Python
# externals, and importing pml also imports efl, objectmodel, and rbbl.
# Ensure that all of them are built.
add_dependencies( ${APPLICATION_NAME} visrpython eflpython pmlpython rbblpython objectmodelpython )

# Encode the current source location to give access to the Python code contained in the test/python directory.
target_compile_definitions( ${APPLICATION_NAME} PRIVATE CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

set_target_properties( ${APPLICATION_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

set_target_properties( ${APPLICATION_NAME} PROPERTIES FOLDER unit_tests )

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         COMMAND ${APPLICATION_NAME} )

include( adjust_test_environment )
# Set up PYTHONPATH to allow finding the VISR Python externals, plus the
# environment needed by the embedded interpreter of the test executable.
adjustEmbeddedPythonTestEnvironment( ${APPLICATION_NAME} visrpython )
