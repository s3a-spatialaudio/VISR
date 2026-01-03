# Copyright Institute of Sound and Vibration Research - All rights reserved

add_test(NAME rrlpython_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         COMMAND ${PYTHON_TEST_VENV_EXECUTABLE} -m pytest )

set_tests_properties(rrlpython_test PROPERTIES
  FIXTURES_REQUIRED visr_test_package
)