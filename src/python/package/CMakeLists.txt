
# Set up a valid package directory structure.
configure_file( pyproject.toml.in ${VISR_PYTHON_PACKAGE_ROOT}/pyproject.toml )
configure_file( setup.cfg.in ${VISR_PYTHON_PACKAGE_ROOT}/setup.cfg )
configure_file( setup.py.in ${VISR_PYTHON_PACKAGE_ROOT}/setup.py )
configure_file( MANIFEST.in.in ${VISR_PYTHON_PACKAGE_ROOT}/MANIFEST.in )
configure_file( ${PROJECT_SOURCE_DIR}/Readme.md ${VISR_PYTHON_PACKAGE_ROOT}/Readme.md )
configure_file( ${PROJECT_SOURCE_DIR}/LICENSE.md ${VISR_PYTHON_PACKAGE_ROOT}/LICENSE.md )
configure_file( __init__.py.in ${VISR_PYTHON_PACKAGE_DIR}/__init__.py )
configure_file( dll_path_context.py ${VISR_PYTHON_PACKAGE_DIR}/impl/dll_path_context.py )
# Create the directory structure of a nameapace package, including __init__.py files that
# import the symbols for each subpackage from an exteranl in visr/impl.
foreach( _external ${VISR_PYTHON_EXTERNALS} )
  set( _SUBPACKAGE_NAME ${_external} )
  configure_file( __init__.py.subpackage.template.in 
    ${VISR_PYTHON_PACKAGE_DIR}/${_external}/__init__.py
  )
endforeach()

# Determine the actual external external file paths.
set( EXTERNAL_TARGET_FILES )
foreach( _external ${VISR_PYTHON_EXTERNALS} )
  list( APPEND EXTERNAL_TARGET_FILES $<TARGET_FILE:${_external}python> )
endforeach()

set( VISR_PYTHON_EXTERNALS_EXTRA_DEPENDENCIES )
if( ${VISR_PYTHON_EXTERNAL_LINK_LIB_TYPE} STREQUAL shared )
foreach( _external ${VISR_PYTHON_EXTERNALS} )
  list( APPEND VISR_PYTHON_EXTERNALS_EXTRA_DEPENDENCIES $<TARGET_FILE:${_external}_shared> )
endforeach()
endif()

# Note: the binary path generation is only needed for the python_package target, but not if the 
# whl package is generated at the install stage.
include(GeneratePythonBinaryWheel)

getBinaryPythonWheelName( visr ${VISR_VERSION} _WHEEL_FILE_NAME )

# Debug output
# message( STATUS "Wheel name: " ${_WHEEL_FILE_NAME} )
# message( STATUS "EXTERNAL_TARGET_FILES: " ${EXTERNAL_TARGET_FILES} )
# message( STATUS "VISR_PYTHON_EXTERNALS_EXTRA_DEPENDENCIES: " ${VISR_PYTHON_EXTERNALS_EXTRA_DEPENDENCIES} )

add_custom_command( OUTPUT ${_WHEEL_FILE_NAME}
  MAIN_DEPENDENCY ${VISR_PYTHON_PACKAGE_ROOT}/pyproject.toml
  WORKING_DIRECTORY ${VISR_PYTHON_PACKAGE_ROOT}
  COMMAND ${PYTHON_EXECUTABLE} -m build --no-isolation --outdir ${PROJECT_BINARY_DIR}/python_packages --wheel
  DEPENDS ${PYTHON_EXTERNALS_TARGETS}
)

add_custom_target( python_package DEPENDS ${_WHEEL_FILE_NAME} )

# Note: Apparently it is not possible to access the source directory in the install stage, therefore we cannot 
# use the same configure_file logic as in the build tree, but have to copy the already generated builf tree.
install( DIRECTORY
${CMAKE_BINARY_DIR}/python
  DESTINATION .
  FILES_MATCHING
    PATTERN "*.py"
    PATTERN "*.md"
    PATTERN "*.toml"
    PATTERN "*.md"
    PATTERN "MANIFEST.in"
)

# TODO: Consider removing the python/ directory after the generation.
install( CODE [===[
  find_package( Python REQUIRED COMPONENTS Interpreter )
  execute_process( 
    COMMAND ${Python_EXECUTABLE} -m build --no-isolation --outdir $<INSTALL_PREFIX> --wheel
    WORKING_DIRECTORY $<INSTALL_PREFIX>/python
    ECHO_OUTPUT_VARIABLE ECHO_ERROR_VARIABLE
    OUTPUT_VARIABLE WHEEL_OUTPUT
    ERROR_VARIABLE WHEEL_ERROR
    RESULT_VARIABLE WHEEL_RESULT
    ECHO_OUTPUT_VARIABLE ECHO_ERROR_VARIABLE
    COMMAND_ECHO STDOUT
    )
  ]===]
)
