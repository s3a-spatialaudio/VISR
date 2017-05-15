# Python bindings for the main VISR interface classes

set( SOURCES
double_buffering_protocol.cpp
empty_parameter_config.cpp
indexed_value_parameter.cpp
filter_routing_parameter.cpp
listener_position.cpp
matrix_parameter.cpp
message_queue_protocol.cpp
object_vector.cpp
pml.cpp
vector_parameter.cpp
)

set( HEADERS
double_buffering_protocol.hpp
empty_parameter_config.hpp
listener_position.hpp
matrix_parameter.hpp
message_queue_protocol.hpp
object_vector.hpp
vector_parameter.hpp
)

set(PROJECT_NAME pmlpython)

set(MODULE_NAME pml)


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

target_link_libraries( ${PROJECT_NAME} pml )

target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARY} )

install(TARGETS ${PROJECT_NAME} DESTINATION ../python)
