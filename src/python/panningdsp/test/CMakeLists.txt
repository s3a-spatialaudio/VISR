# Copyright Institute of Sound and Vibration Research - All rights reserved

add_test(NAME panningdsppython_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         COMMAND ${Python_EXECUTABLE} -m pytest )

set_tests_properties( panningdsppython_test PROPERTIES
  FIXTURES_REQUIRED visr_test_package
)
