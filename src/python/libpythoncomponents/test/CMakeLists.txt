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

add_dependencies(${APPLICATION_NAME} python_test_package)

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

# Work around the broken support of virtual environments from embedded
# interpreters on Windows (see https://bugs.python.org/issue22213#msg366570).
# That is, neither having the venv directory on the %Path% nor setting
# %PYTHONHOME% works.
# Therefore we need to use the system Python installation and install the visr package locally.
# This mean the test changes the system Python installation (by installing the present visr 
# package and thus removing a prior version of that package.)
if(VISR_SYSTEM_NAME MATCHES "Windows")
  include( adjust_test_environment )
  adjustTestEnvironment(${APPLICATION_NAME})

  # Note: We use ${PYTHON_EXECUTABLE} to get the Python interpreter before creating the virtual
  # environment python_test_venv, as opposed to ${Python_EXECUTABLE} which gets the interpreter
  # of this venv. This is fragile because it relies on the details how pybind11 finds Python.
  # A more resilient implementation to store the "old" interpreter in a variable before 
  # re-finding Python ofter the creation of the venv.
  add_test(NAME python_test_install_visr_windows_embedded_fixture
    COMMAND ${PYTHON_EXECUTABLE} -m pip install --force-reinstall ${_EXTERNAL_PACKAGE_NAME}
  )
  set_tests_properties(python_test_install_visr_windows_embedded_fixture PROPERTIES
    FIXTURES_SETUP windows_install_visr_python_package_to_system_python)
  # Note: There is no cleanup job because this would remove a previously installed 
  # VISR package.

  set_tests_properties(${APPLICATION_NAME} PROPERTIES FIXTURES_REQUIRED
    windows_install_visr_python_package_to_system_python)
else()
  # Prepend the directory of the current Python interpreter to the path to ensure
  # that the right Python environment is loaded. This is the primary mechanism to
  # locate the correct Python environment although it has shortcomings, see
  # https://bugs.python.org/issue22213#msg225434 .
  # This might be a path to a virtual environment.
  get_filename_component(_PYTHON_EXECTUABLE_PATH ${Python_EXECUTABLE} DIRECTORY)
    set_property(TEST ${APPLICATION_NAME} APPEND PROPERTY ENVIRONMENT
      "PATH=${_PYTHON_EXECTUABLE_PATH}:$ENV{PATH}")

  set_tests_properties(${APPLICATION_NAME} PROPERTIES FIXTURES_REQUIRED visr_test_package)
endif(VISR_SYSTEM_NAME MATCHES "Windows")

# Disable leak checking because the memory handling of Python interpreter
# causes leak reports (false positives?) that are outside our responsibility.
set_property(TEST ${APPLICATION_NAME} APPEND PROPERTY ENVIRONMENT
  "BOOST_TEST_DETECT_MEMORY_LEAK=0" )
