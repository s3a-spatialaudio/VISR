# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME audiointerfaces_test )

# Make the current source directory known to the source code to create absolute file paths.
add_definitions( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )
add_definitions( -DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" )

add_executable( ${APPLICATION_NAME} 
audio_interface_configuration.cpp
)

target_link_libraries( ${APPLICATION_NAME} audiointerfaces_static )
target_link_libraries( ${APPLICATION_NAME} rcl_static )
target_link_libraries( ${APPLICATION_NAME} rrl_static )
target_link_libraries( ${APPLICATION_NAME} Boost::unit_test_framework )
target_link_libraries( ${APPLICATION_NAME} ${CMAKE_THREAD_LIBS_INIT} )
if( NOT Boost_USE_STATIC_LIBS )
  target_compile_definitions(${APPLICATION_NAME} PRIVATE -DBOOST_ALL_DYN_LINK )
endif( NOT Boost_USE_STATIC_LIBS )
target_compile_definitions( ${APPLICATION_NAME} PRIVATE -DBOOST_ALL_NO_LIB )
set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
