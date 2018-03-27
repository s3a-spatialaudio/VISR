# Python bindings for the main VISR interface classes

set( SOURCES
atomic_component.cpp
audio_connection.cpp
audio_port.cpp
audio_sample_type.cpp
channel_list.cpp
communication_protocol.cpp
communication_protocol_factory.cpp
component.cpp
composite_component.cpp
parameter_base.cpp
parameter_config_base.cpp
parameter_connection.cpp
parameter_factory.cpp
parameter_port.cpp
port.cpp
signal_flow_context.cpp
visr.cpp
)

set( HEADERS
)

set(PROJECT_NAME visrpython)

set(MODULE_NAME visr )

########################################################################
# Internal logic, do not edit.

include_directories(${PYTHON_INCLUDE_DIR})

# Set the filename extension of Python external modules if it differs from the 
# standard extensions for modules on this platform.
# TODO: This might be moved to a central location
if( WIN32 )
  set( PYTHON_EXTERNAL_SUFFIX ".pyd" )
endif( WIN32 )

# Whether to use the pybind11 build support or the standard 'module' shared library.
add_library(${PROJECT_NAME} MODULE ${HEADERS} ${SOURCES} )
# pybind11_add_module( ${PROJECT_NAME} MODULE ${HEADERS} ${SOURCES} )


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


target_link_libraries( ${PROJECT_NAME} visr_shared )
target_link_libraries( ${PROJECT_NAME} ${PYTHON_LIBRARY} )

# ??? Correct? THE LIBRARY_OUTPUT_DIRECTORY should be enough for the compilation.
# Packaging has to be handled specifically.
install(TARGETS ${PROJECT_NAME} DESTINATION ${PYTHON_MODULE_INSTALL_DIRECTORY})
