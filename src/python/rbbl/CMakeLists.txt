# Python bindings for the standard VISR interface component

set( SOURCES
multichannel_convolver_uniform.cpp
rbbl.cpp
)

# We don't need header files

set(PROJECT_NAME rbblpython)

set(MODULE_NAME rbbl)


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

target_link_libraries( ${PROJECT_NAME} rbbl_shared )

# Note: Due to a circular dependency, the transitive dependency of
# rbbl_shared on pml_shared is omitted in rbbl_shared's CMakelists.txt.
# See comment there. As a result, we need to state it here.
target_link_libraries( ${PROJECT_NAME} pml_shared )

target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARY} )

install(TARGETS ${PROJECT_NAME} DESTINATION python)
