# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( APPLICATION_NAME visr_impl_test )

SET( SOURCES
audio_connection_descriptor.cpp
audio_sample_type.cpp
component_names.cpp
parameter_factory.cpp
test_main.cpp
)

ADD_EXECUTABLE( ${APPLICATION_NAME} ${SOURCES} )

TARGET_LINK_LIBRARIES( ${APPLICATION_NAME} visr_impl)
TARGET_LINK_LIBRARIES( ${APPLICATION_NAME} ${WHOLE_ARCHIVE_PREFIX} pml ${WHOLE_ARCHIVE_SUFFIX} )
TARGET_LINK_LIBRARIES( ${APPLICATION_NAME} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )

set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
