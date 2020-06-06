# Copyright Institute of Sound and Vibration Research - All rights reserved

add_definitions( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )
add_definitions( -DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" )

set( SOURCES
allrad.cpp
cap.cpp
loudspeaker_array.cpp
vbap.cpp
test_main.cpp
)

set( APPLICATION_NAME panning_test )

add_executable( ${APPLICATION_NAME} ${SOURCES} )

target_link_libraries( ${APPLICATION_NAME} PRIVATE panning_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries( ${APPLICATION_NAME} PRIVATE pml_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries( ${APPLICATION_NAME} PRIVATE Boost::filesystem )
target_link_libraries( ${APPLICATION_NAME} PRIVATE Boost::unit_test_framework )
if( NOT Boost_USE_STATIC_LIBS )
  target_compile_definitions(${APPLICATION_NAME} PRIVATE -DBOOST_ALL_DYN_LINK )
endif( NOT Boost_USE_STATIC_LIBS )
target_compile_definitions( ${APPLICATION_NAME} PRIVATE -DBOOST_ALL_NO_LIB )

set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

# TODO: Remove this, because this should already be a transitive dependency 
if( BUILD_USE_IPP )
  target_link_libraries(${APPLICATION_NAME} ${IPP_LIB_S} ${IPP_LIB_CORE} )
endif( BUILD_USE_IPP )

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )

# On Windows, we need to prepend the lib/ directory and the 3rd-party directory to the PATH in order
# to find the runtime dependencies. 
if(VISR_SYSTEM_NAME MATCHES "Windows")
  # Prevent CMake from interpreting the semicolon (the separator in Windows path lists) as list separator.
  # Double quoting required because the variable is evaluated twice.
  string(REPLACE ";" "\\\;"  QUOTED_PATH "$ENV{PATH}" )
  if( VISR_LIBRARY_TYPE_FOR_APPS STREQUAL "shared" )
    set_property(TEST ${APPLICATION_NAME} PROPERTY ENVIRONMENT
      "PATH=${VISR_BUILD_3RD_PARTY_RUNTIME_LIBRARY_DIR}\;$<TARGET_FILE_DIR:visr_shared>\;${QUOTED_PATH}" )
  else()
   set_property(TEST ${APPLICATION_NAME} PROPERTY ENVIRONMENT
      "PATH=${VISR_BUILD_3RD_PARTY_RUNTIME_LIBRARY_DIR}\;${QUOTED_PATH}" )
  endif( VISR_LIBRARY_TYPE_FOR_APPS STREQUAL "shared" )
endif(VISR_SYSTEM_NAME MATCHES "Windows")
