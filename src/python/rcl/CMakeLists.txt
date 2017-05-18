# Python bindings for the standard VISR interface component

set( SOURCES
add.cpp
delay_vector.cpp
fir_filter_matrix.cpp
panning_calculator.cpp
rcl.cpp
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

target_link_libraries( ${PROJECT_NAME} rcl ) 

target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARY} )

install(TARGETS ${PROJECT_NAME} DESTINATION python)
