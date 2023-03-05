# Support for creating Python binary packages from C++ externals

find_package( Python3 REQUIRED )

# Helper function to compute the resulting name of a binary wheel.
function( getBinaryPythonWheelName distName version result )
  execute_process( COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/python/get_binary_python_wheel_name.py ${distName} --version=${version}
    RESULTS_VARIABLE _RETURN_VALUE
    OUTPUT_VARIABLE _FILE_NAME
  )
  if( NOT ${_RETURN_VALUE} EQUAL 0 )
    message( SEND_ERROR "getBinaryPythonWheelName returned error." )
  endif()
  set( ${result} ${_FILE_NAME} PARENT_SCOPE )
endfunction()


# Function to define a CMake target for building a wheel (binary installation package).
#
# Parameters:
# targetName: The name of the generated target.
# sourceDir: The source directory containing the setup.py function
# distName: The name of the Python package, using dots to represent a namespace package.
# version: Semantic version string, like 0.1.2
# buildType: The CMake build type, for example Release, Debug, RelWithDebInfo. (Note: this currently
# works only with single-config generators, not with multi-config generators like Visual studio or Xcode). 
# Additional arguments are passed as CMake definitions to the setup method. They must have the form
# KEY=value.
# TODO: Pass the build configuration as a command-line to setup.py (rather than writing it during 
# configure_file). This will probably require a change in cmake-build-extension.
function( generatePythonBinaryWheel targetName sourceDir distName version )
  getBinaryPythonWheelName( ${distName} ${version} _WHEEL_FILE )

  # Create list of defines
  set( _DEFINITION_STRING "" )
  if( ${ARGC} GREATER 4 )
    math( EXPR _endIndex "${ARGC} - 1" )
    set( _DEFINITION_STRING "-D\"" )
    foreach( _argIdx RANGE 4 ${_endIndex})
      set( _arg ${ARGV${_argIdx}} )
      if( ${_argIdx} LESS ${_endIndex} )
        set( _DEFINITION_STRING "${_DEFINITION_STRING}${_arg}\\;" )
      else()
        set( _DEFINITION_STRING "${_DEFINITION_STRING}${_arg}\"" )
      endif()
    endforeach()
  endif()
  # message( STATUS "generatePythonBinaryWheel definition string: " ${_DEFINITION_STRING} )
  add_custom_command( OUTPUT ${_WHEEL_FILE} 
    COMMAND ${PYTHON_EXECUTABLE} ${sourceDir}/setup.py bdist_wheel build_ext ${_DEFINITION_STRING}
  )
  add_custom_target( ${targetName}
    DEPENDS ${_WHEEL_FILE}
  )
  set_target_properties( ${targetName} PROPERTIES
   ADDITIONAL_CLEAN_FILES ${CMAKE_CURRENT_BINARY_DIR}/build\;${CMAKE_CURRENT_BINARY_DIR}/dist
  )

endfunction()
