# Copyright Institute of Sound and Vibration Research - All rights reserved

add_test(NAME eflpython_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${PYTHON_EXECUTABLE} -m pytest )
set_tests_properties( eflpython_test PROPERTIES ENVIRONMENT PYTHONPATH=$<TARGET_FILE_DIR:eflpython> )
