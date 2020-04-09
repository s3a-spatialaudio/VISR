# Copyright Institute of Sound and Vibration Research - All rights reserved

add_test(NAME rclpython_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         COMMAND ${PYTHON_EXECUTABLE} -m pytest )
set_tests_properties( rclpython_test PROPERTIES ENVIRONMENT PYTHONPATH=$<TARGET_FILE_DIR:rclpython> )

# On Windows, we need to prepend the lib/ directory and the 3rd-party directory to the PATH in order
# to find the runtime dependencies.
# TODO: Consider to implement this as a function in a central location, because
# it is needed by all targets that access Python externals during the build process,
# e.g., unit tests and the Sphinx-based documentation.
# Note: This evaluates the value of the $PATH variable at CMake configure time, and does therefore not
# reflect changes done between that and the test execution.
if(VISR_SYSTEM_NAME MATCHES "Windows")
  # Prevent CMake from interpreting the semicolon (the separator in Windows path lists) as list separator.
  # Double quoting required because the variable is evaluated twice.
  string(REPLACE ";" "\\\;"  QUOTED_PATH "$ENV{PATH}" )
  set_property(TEST rclpython_test PROPERTY ENVIRONMENT "PATH=${VISR_BUILD_3RD_PARTY_RUNTIME_LIBRARY_DIR}\;$<TARGET_FILE_DIR:rcl_shared>\;${QUOTED_PATH}" )
endif(VISR_SYSTEM_NAME MATCHES "Windows")
