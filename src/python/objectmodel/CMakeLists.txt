# Python bindings for the main VISR interface classes

set( SOURCES
hoa_source.cpp
object.cpp
object_type.cpp
object_vector.cpp
objectmodel.cpp
plane_wave.cpp
point_source.cpp
point_source_with_reverb.cpp
)

set( HEADERS
)

# We don't need header files

set(PROJECT_NAME objectmodelpython)

set(MODULE_NAME objectmodel)


########################################################################
# Internal logic, do not edit.

include_directories(${PYTHON_INCLUDE_DIR})

# Set the filename extension of Python external modules if it differs from the 
# standard extensions for modules on this platform.
# TODO: This might be moved to a central location
if( WIN32 )
  set( PYTHON_EXTERNAL_SUFFIX ".pyd" )
endif( WIN32 )

add_library(${PROJECT_NAME} MODULE ${HEADERS} ${SOURCES} )

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${MODULE_NAME} )
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" )
if( PYTHON_EXTERNAL_SUFFIX )
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ${PYTHON_EXTERNAL_SUFFIX} )
endif( PYTHON_EXTERNAL_SUFFIX )
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/python" )

target_link_libraries( ${PROJECT_NAME} objectmodel_shared )
target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARY} )

install(TARGETS ${PROJECT_NAME} DESTINATION python)
