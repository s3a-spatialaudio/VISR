# Python bindings for the standard VISR interface component

set( SOURCES
add.cpp
biquad_iir_filter.cpp
crossfading_fir_filter_matrix.cpp
delay_matrix.cpp
delay_vector.cpp
fir_filter_matrix.cpp
gain_matrix.cpp
gain_vector.cpp
hoa_allrad_gain_calculator.cpp
panning_calculator.cpp
rcl.cpp
scene_decoder.cpp
scalar_osc_decoder.cpp
signal_routing.cpp
time_frequency_inverse_transform.cpp
time_frequency_transform.cpp
udp_receiver.cpp
)

# We don't need header files

set(PROJECT_NAME rclpython)

set(MODULE_NAME rcl)


########################################################################
# Internal logic, do not edit.

include_directories(${PYTHON_INCLUDE_DIR})

# Set the filename extension of Python external modules if it differs from the 
# standard extensions for modules on this platform.
# TODO: This might be moved to a central location
if( WIN32 )
  set( PYTHON_EXTERNAL_SUFFIX ".pyd" )
endif( WIN32 )

add_library(${PROJECT_NAME} MODULE ${SOURCES} )

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${MODULE_NAME} )
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" )
if( PYTHON_EXTERNAL_SUFFIX )
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ${PYTHON_EXTERNAL_SUFFIX} )
endif( PYTHON_EXTERNAL_SUFFIX )
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/python" )
# In case of Windows, we need to make sure that the import library part does not conflict with a static library of the same name.
# So we leave it in the build directory and add an _exprt suffix.
set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_NAME "${PROJECT_NAME}_import" )

target_link_libraries( ${PROJECT_NAME} rcl_shared ) 

target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARY} )

install(TARGETS ${PROJECT_NAME} DESTINATION python)
