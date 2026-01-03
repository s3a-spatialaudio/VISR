# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME pythoncomponents_test )

set( SOURCES
pml_initialisation.cpp
test_main.cpp
wrapper.cpp
)

add_executable( ${APPLICATION_NAME} ${SOURCES} )

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

# Since the 'wrapper' unit tests require the visr Python external, ensure that it is compiled
add_dependencies( ${APPLICATION_NAME} visrpython eflpython pmlpython rbblpython objectmodelpython )

# Encode the current source location to give access to the Python code contained in the test/python directory.
target_compile_definitions( ${APPLICATION_NAME} PRIVATE CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

set_target_properties( ${APPLICATION_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

set_target_properties( ${APPLICATION_NAME} PROPERTIES FOLDER unit_tests )

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )

# Prepend the directory of the current Python interpreter to the path to ensure
# that the right Python environment is loaded.
# This might be a path to a virtual environment.
message(STATUS "PYTHON_TEST_VENV_EXECUTABLE " ${PYTHON_TEST_VENV_EXECUTABLE})

get_filename_component(_PYTHON_EXECTUABLE_PATH ${PYTHON_TEST_VENV_EXECUTABLE} DIRECTORY)
if(VISR_SYSTEM_NAME MATCHES "Windows")
  # Prevent CMake from interpreting the semicolon (the separator in Windows 
  # path lists) as list separator.
  # Double quoting required because the variable is evaluated twice.
  string(REPLACE ";" "\\\;"  QUOTED_PATH "$ENV{PATH}" )
  set_property(TEST ${APPLICATION_NAME} APPEND PROPERTY ENVIRONMENT
    "PATH=${_PYTHON_EXECTUABLE_PATH}\;${QUOTED_PATH}" )
else()
  set_property(TEST ${APPLICATION_NAME} APPEND PROPERTY ENVIRONMENT
    "PATH=${_PYTHON_EXECTUABLE_PATH}:$ENV{PATH}" )
endif()

# Disable leak checking because the memory handling of Python interpreter
# causes leak reports (false positives?) that are outside our responsibility.
set_property(TEST ${APPLICATION_NAME} APPEND PROPERTY ENVIRONMENT
  "BOOST_TEST_DETECT_MEMORY_LEAK=0" )

set_tests_properties(${APPLICATION_NAME} PROPERTIES FIXTURES_REQUIRED visr_test_package)
