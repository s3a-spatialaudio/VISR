# Copyright Institute of Sound and Vibration Research - All rights reserved

# Set the directory containing the 'visr' python package data structure.
# By adding this directory to the Python module search path, the VISR package
# can be imported without being installed.
# TODO: COnsider moving this definition up because the same directory is used
# by the libpythoncomponents unit test.
set( VISR_PYTHON_PACKAGE_ROOT ${CMAKE_BINARY_DIR}/python/src )

# Define the output directory for the built externals.
# TODO: Make this work with multi-configuration targets!
# Note: The $<0:> suppresses the build type directory appended for multi-configuration targets.
set(PYTHON_EXTERNAL_OUTPUT_DIRECTORY "${VISR_PYTHON_PACKAGE_ROOT}/visr/impl/$<0:>")

# Define an output directory for the 'import lib stub' created for every Python external.
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

# List containing the target names of all Python externals.
set( VISR_PYTHON_EXTERNALS_TARGETS )

# Iterate over all externals to define the targets and add them to the target list.
foreach( _external ${VISR_PYTHON_EXTERNALS} )
  add_subdirectory( ${_external} )
  set( _externalTarget ${_external}python )
  list( APPEND VISR_PYTHON_EXTERNALS_TARGETS ${_externalTarget} )
  # Skip the RPATH for the Python external, because only shared libraries (incl. other externals)
  # in the current directory will be used.
  set_target_properties( ${_externalTarget} PROPERTIES SKIP_BUILD_RPATH TRUE )
endforeach()

# "Meta-target" combining all Python externals to serve as a single dependency.
add_custom_target( pythonexternals DEPENDS ${VISR_PYTHON_EXTERNALS_TARGETS} )
set_target_properties( pythonexternals PROPERTIES FOLDER python_externals )

# Subdirectory containing build instructions for the visr Python namespace package
# incl. binary distribution packages (wheels).
add_subdirectory( package )
