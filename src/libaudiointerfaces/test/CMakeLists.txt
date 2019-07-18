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
if( NOT Boost_USE_STATIC_LIBS )
  target_compile_definitions(${APPLICATION_NAME} PRIVATE -DBOOST_ALL_DYN_LINK )
endif( NOT Boost_USE_STATIC_LIBS )
target_compile_definitions( ${APPLICATION_NAME} PRIVATE -DBOOST_ALL_NO_LIB )
set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )

# On Windows, we need to prepend the lib/ directory and the 3rd-party directory to the PATH in order
# to find the runtime dependencies. 
# Note: This evaluates the value of the $PATH variable at CMake configure time, and does therefore not
# reflect changes done between that and the test execution.
if(VISR_SYSTEM_NAME MATCHES "Windows")
  # Prevent CMake from interpreting the semicolon (the separator in Windows path lists) as list separator.
  # Double quoting required because the variable is evaluated twice.
  string(REPLACE ";" "\\\;"  QUOTED_PATH "$ENV{PATH}" )
  set_property(TEST ${APPLICATION_NAME} PROPERTY ENVIRONMENT "PATH=${VISR_BUILD_3RD_PARTY_RUNTIME_LIBRARY_DIR}\;$<TARGET_FILE_DIR:visr_shared>\;${QUOTED_PATH}" )
endif(VISR_SYSTEM_NAME MATCHES "Windows")
