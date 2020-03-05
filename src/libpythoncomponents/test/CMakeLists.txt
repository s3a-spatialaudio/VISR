# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME pythoncomponents_test )

set( SOURCES
pml_initialisation.cpp
test_main.cpp
wrapper.cpp
)

add_executable( ${APPLICATION_NAME} ${SOURCES} ) 

target_include_directories( ${APPLICATION_NAME} PRIVATE ${PYTHON_INCLUDE_DIR} )
# The pybind11 header is used to include Python.h.
# This avoids preprocessor magic to work around platform specifics.
target_include_directories( ${APPLICATION_NAME} PRIVATE ${PYBIND11_INCLUDE_DIR} )

target_link_libraries( ${APPLICATION_NAME} PRIVATE pythoncomponents_shared )
target_link_libraries( ${APPLICATION_NAME} PRIVATE pythonsupport_shared )
target_link_libraries( ${APPLICATION_NAME} PRIVATE rrl_shared )
target_link_libraries( ${APPLICATION_NAME} PRIVATE pml_shared )
target_link_libraries( ${APPLICATION_NAME} PRIVATE pybind11 )
target_link_libraries( ${APPLICATION_NAME} PRIVATE Boost::filesystem )
target_link_libraries( ${APPLICATION_NAME} PRIVATE Boost::unit_test_framework )
target_link_libraries( ${APPLICATION_NAME} PRIVATE ${PYTHON_LIBRARY} )

if( NOT Boost_USE_STATIC_LIBS )
  target_compile_definitions(${APPLICATION_NAME} PRIVATE -DBOOST_ALL_DYN_LINK )
endif( NOT Boost_USE_STATIC_LIBS )
target_compile_definitions( ${APPLICATION_NAME} PRIVATE -DBOOST_ALL_NO_LIB )

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
set_property(TEST ${APPLICATION_NAME} PROPERTY ENVIRONMENT "PYTHONPATH=$<TARGET_FILE_DIR:visrpython>" )

# On Windows, we need to prepend the lib/ directory and the 3rd-party directory to the PATH in order
# to find the runtime dependencies.
# Note: This evaluates the value of the $PATH variable at CMake configure time, and does therefore not
# reflect changes done between that and the test execution.
if(VISR_SYSTEM_NAME MATCHES "Windows")
  # Prevent CMake from interpreting the semicolon (the separator in Windows path lists) as list separator.
  # Double quoting required because the variable is evaluated twice.
  string(REPLACE ";" "\\\;"  QUOTED_PATH "$ENV{PATH}" )
  set_property(TEST ${APPLICATION_NAME} PROPERTY ENVIRONMENT "PATH=${VISR_BUILD_3RD_PARTY_RUNTIME_LIBRARY_DIR}\;$<TARGET_FILE_DIR:visr_shared>\;${QUOTED_PATH}" )
endif(VISR_SYSTEM_NAME MATCHES "Windows")
