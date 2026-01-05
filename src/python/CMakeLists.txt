# Copyright Institute of Sound and Vibration Research - All rights reserved

# Set the top-level directory containing the generated Python directory structure.
# This is used as the working directory for the package generation command.,
set( VISR_PYTHON_PACKAGE_ROOT ${CMAKE_BINARY_DIR}/python )

# Set the directory containing the 'visr' python package data structure.
# By adding this directory to the Python module search path, the VISR package
# can be imported without being installed.
# TODO: Consider moving this definition up because the same directory is used
# by the libpythoncomponents unit test.
set( VISR_PYTHON_PACKAGE_SRC ${VISR_PYTHON_PACKAGE_ROOT}/src )

# The location of the 'visr' Python package.
set( VISR_PYTHON_PACKAGE_DIR ${VISR_PYTHON_PACKAGE_SRC}/visr )

# Define the output directory for the built externals.
# TODO: Make this work with multi-configuration targets!
# Note: The $<0:> suppresses the build type directory appended for multi-configuration targets.
set(PYTHON_EXTERNAL_OUTPUT_DIRECTORY "${VISR_PYTHON_PACKAGE_DIR}/impl/$<0:>")

# Define an output directory for the 'import lib stub' created for every Python
#  external on Windows.
# This prevents them from colliding with static libraries of the same name.
# TODO: Is it possible that we need to install them them in certain circumstances
# (e.g., linking dependent externals in other projects).
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/importlib)

# List the externals to be included in the package.
# These are also the subdirectory names.
set( VISR_PYTHON_EXTERNALS
  audiointerfaces
  efl
  objectmodel
  panning
  panningdsp
  pml
  rbbl
  rcl
  rrl
  reverbobject
  signalflows
  visr
)

# This is not possible because the static boost libraries are normally not
# compiled as position-independent code, which means they cannot used in shared
# objects as Python externals.
# This definitely leads to link errors on Linux and MacOS, but it should
# conceptually be the same problem on Windows.
if( Boost_USE_STATIC_LIBS AND NOT WIN32 )
  message( WARNING
   "Build Python bindings when using static boost libraries will fail if libraries are not position-independent." )
endif( Boost_USE_STATIC_LIBS AND NOT WIN32 )

set( BUILD_PYTHON_EXTERNALS_LINK_LIBRARY_TYPE "shared" CACHE STRING "The link library type used by Python externals." )
set( VISR_PYTHON_EXTERNALS_LINK_LIBRARY_TYPES shared static_pic )
set_property( CACHE BUILD_PYTHON_EXTERNALS_LINK_LIBRARY_TYPE PROPERTY STRINGS ${VISR_PYTHON_EXTERNALS_LINK_LIBRARY_TYPES} )

set( VISR_PYTHON_EXTERNAL_LINK_LIB_TYPE ${BUILD_PYTHON_EXTERNALS_LINK_LIBRARY_TYPE} )

if( NOT ${VISR_PYTHON_EXTERNAL_LINK_LIB_TYPE} IN_LIST VISR_BUILD_LIBRARY_TYPES )
  message( SEND_ERROR "The library type for linking Python externals (" ${VISR_PYTHON_EXTERNAL_LINK_LIB_TYPE}
    ") is not being built." )
endif()

# Define helper library used by several Python externals.
add_subdirectory( libpythonbindinghelpers )

# Query whether the CMake generator is a multi-configuration generator.
# This is used below because paths differ between single- and multi-
# configuration configurators.
get_property(VISR_BUILD_MULTI_CONFIG_GENERATOR GLOBAL 
  PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(BUILD_TESTING)
  # Note: At the moment, the test environment venv is created at configure time
  # because creating it at build time would make setting the interpreter difficult
  # (how to pass the interpreter pathe back to CMake?).

  message(STATUS "Python test venv creation started.")
  set(_TEST_VENV_NAME "python_test_venv")
  set(_TEST_VENV_LOCATION ${CMAKE_BINARY_DIR}/${_TEST_VENV_NAME})
  set(_TEST_VENV_STAMP ${_TEST_VENV_LOCATION}/pyvenv.cfg)
  set(_TEST_VENV_REQUIREMENTS_FILE ${CMAKE_CURRENT_DIRECTORY}/package/requirements.txt})
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -m venv --system-site-packages ${_TEST_VENV_NAME}
    COMMAND_ERROR_IS_FATAL ANY
  )
    message(STATUS "Python test venv created.")

  # Taken from https://discourse.cmake.org/t/possible-to-create-a-python-virtual-env-from-cmake-and-then-find-it-with-findpython3/1132/2
  # Here is the trick
  ## update the environment with VIRTUAL_ENV variable (mimic the activate script)
  set(ENV{VIRTUAL_ENV} ${_TEST_VENV_LOCATION})
  ## change the context of the search
  set(Python_FIND_VIRTUALENV FIRST)
  ## unset Python_EXECUTABLE because it is also an input variable (see documentation, Artifacts Specification section)
  unset(Python_EXECUTABLE)
  ## Launch a new search
  find_package (Python COMPONENTS Interpreter Development)
  # Define a dedicated name for the Python interpreter to be used in the unit tests
  set(PYTHON_TEST_VENV_EXECUTABLE ${Python_EXECUTABLE})
  message(STATUS "Python executable to be used in unit tests: " ${PYTHON_TEST_VENV_EXECUTABLE})

  set(_TEST_VENV_REQUIREMENTS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/package/requirements.txt)
  execute_process(
    COMMAND ${PYTHON_TEST_VENV_EXECUTABLE} -m pip install -r ${_TEST_VENV_REQUIREMENTS_FILE}
    COMMAND_ERROR_IS_FATAL ANY
    )

  # Determine the name of the created wheel file.
  include(GeneratePythonBinaryWheel)
  getBinaryPythonWheelName( visr ${VISR_VERSION} _TEST_WHEEL_FILE_NAME )
  message( STATUS "Test wheel name: " ${_TEST_WHEEL_FILE_NAME} )

  if(${VISR_BUILD_MULTI_CONFIG_GENERATOR})
    message(STATUS "Test environment will be built for multi-configuration generator.")
    set(_TEST_ENV_PREFIX ${CMAKE_BINARY_DIR}/tmp_install/$<CONFIG>)
    set(_EXTERNAL_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/packages/$<CONFIG>)
  else()
    message(STATUS "Test environment will be built for single-configuration generator.")
    set(_TEST_ENV_PREFIX ${CMAKE_BINARY_DIR}/tmp_install)
    set(_EXTERNAL_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
  endif()
  set(_EXTERNAL_PACKAGE_NAME ${_EXTERNAL_OUTPUT_DIRECTORY}/${_TEST_WHEEL_FILE_NAME})

  add_test(NAME python_test_install_visr_fixture
    COMMAND ${PYTHON_TEST_VENV_EXECUTABLE} -m pip install ${_EXTERNAL_PACKAGE_NAME}
  )
  set_tests_properties(python_test_install_visr_fixture PROPERTIES
    FIXTURES_SETUP visr_test_package)

  add_test(NAME python_test_uninstall_visr_fixture
    COMMAND ${PYTHON_TEST_VENV_EXECUTABLE} -m pip uninstall -y visr
  )
  set_tests_properties(python_test_uninstall_visr_fixture PROPERTIES
    FIXTURES_CLEANUP visr_test_package)
endif()

# Python support for calling Python functionality from C++.
add_subdirectory( libpythonsupport )
# C++ VISR component that contains a Python VISR component.
add_subdirectory( libpythoncomponents )
list( APPEND VISR_LIBRARIES pythonsupport pythoncomponents )

# List containing the target names of all Python externals.
set( VISR_PYTHON_EXTERNALS_TARGETS )

# Iterate over all externals to define the targets and add them to the target list.
foreach( _external ${VISR_PYTHON_EXTERNALS} )
  add_subdirectory( ${_external} )
  set( _externalTarget ${_external}python )
  list( APPEND VISR_PYTHON_EXTERNALS_TARGETS ${_externalTarget} )

  if( AUTOSET_RPATH )
    # Adjust the rpath of the Python externals to contain only the current directory
    # because the VISR shared libraries and their runtime dependencies are installed
    # into this folder
    if( ${VISR_SYSTEM_NAME} STREQUAL "Linux" )
        # Use new CMake behaviour without CMake-style escaping for rpaths if available.
      if( POLICY CMP0095 )
        set_target_properties( ${_externalTarget} PROPERTIES INSTALL_RPATH "\${ORIGIN}" )
      else()
        set_target_properties( ${_externalTarget} PROPERTIES INSTALL_RPATH "\\\${ORIGIN}" )
      endif()
    elseif( ${VISR_SYSTEM_NAME} STREQUAL "MacOS" )
      set_target_properties( ${_externalTarget} PROPERTIES INSTALL_RPATH "@loader_path" )
    endif()
  endif()
endforeach()

# "Meta-target" combining all Python externals to serve as a single dependency.
add_custom_target( pythonexternals DEPENDS ${VISR_PYTHON_EXTERNALS_TARGETS} )
set_target_properties( pythonexternals PROPERTIES FOLDER python_externals )

# Subdirectory containing build instructions for the visr Python namespace package
# incl. binary distribution packages (wheels).
add_subdirectory( package )

## --------------------------------------------------------------------------

# Note: We have to create these targets before traversing the subdirectories
# because the unit test targets depend on the test environment

set(_TEST_ENV_STAMP ${_TEST_ENV_PREFIX}/python/pyproject.toml)
if(${VISR_BUILD_MULTI_CONFIG_GENERATOR})
  add_custom_command( OUTPUT ${_TEST_ENV_STAMP}
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR}
          --component python_externals
          --prefix ${_TEST_ENV_PREFIX}
          --config $<CONFIG>
    COMMAND ${CMAKE_COMMAND} -E chdir ${_TEST_ENV_PREFIX}/python 
      ${Python_EXECUTABLE} -m build --no-isolation --outdir ${_EXTERNAL_OUTPUT_DIRECTORY} --wheel
    DEPENDS pythonexternals signalflowspython_shared
  )
else()
  add_custom_command( OUTPUT ${_TEST_ENV_STAMP}
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR}
          --component python_externals
          --prefix ${_TEST_ENV_PREFIX}
    COMMAND ${CMAKE_COMMAND} -E chdir ${_TEST_ENV_PREFIX}/python 
      ${Python_EXECUTABLE} -m build --no-isolation --outdir ${_EXTERNAL_OUTPUT_DIRECTORY} --wheel
    DEPENDS pythonexternals # signalflowspython_shared
  )
endif()

add_custom_target(python_test_package ALL DEPENDS ${_TEST_ENV_STAMP})
