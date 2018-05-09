# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME objectmodel_test )

# Make the current source directory known to the source code to create absolute file paths.
add_definitions( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

add_executable( ${APPLICATION_NAME} 
audio_object_parser.cpp
channel_object_parser.cpp
instantiation.cpp
object_vector_assign.cpp
point_source_with_reverb.cpp
test_main.cpp 
)

target_link_libraries( ${APPLICATION_NAME} objectmodel_static )
target_link_libraries( ${APPLICATION_NAME} Boost::unit_test_framework )
target_link_libraries( ${APPLICATION_NAME} Boost::filesystem )
target_link_libraries( ${APPLICATION_NAME} Boost::system )
target_link_libraries( ${APPLICATION_NAME} ${CMAKE_THREAD_LIBS_INIT} )


set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
