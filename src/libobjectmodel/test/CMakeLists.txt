# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( APPLICATION_NAME objectmodel_test )

# Make the current source directory known to the source code to create absolute file paths.
ADD_DEFINITIONS( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

ADD_EXECUTABLE( ${APPLICATION_NAME} audio_object_parser.cpp instantiation.cpp object_vector_assign.cpp point_source_with_reverb.cpp test_main.cpp )

TARGET_LINK_LIBRARIES( ${APPLICATION_NAME} objectmodel )

TARGET_LINK_LIBRARIES( ${APPLICATION_NAME} pml )

TARGET_LINK_LIBRARIES( ${APPLICATION_NAME} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )

TARGET_LINK_LIBRARIES( ${APPLICATION_NAME} ${Boost_FILESYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES( ${APPLICATION_NAME} ${Boost_SYSTEM_LIBRARY} )

set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
