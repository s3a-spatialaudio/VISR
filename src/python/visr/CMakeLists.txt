# Python bindings for the main VISR interface classes

set( SOURCES
atomic_component.cpp
audio_port.cpp
channel_list.cpp
communication_protocol.cpp
communication_protocol_factory.cpp
component.cpp
composite_component.cpp
parameter_base.cpp
parameter_config_base.cpp
parameter_port.cpp
signal_flow_context.cpp
visr.cpp
)

set( HEADERS
atomic_component.hpp
audio_port.hpp
channel_list.hpp
communication_protocol.hpp
communication_protocol_factory.hpp
component.hpp
composite_component.hpp
parameter_base.hpp
parameter_config_base.hpp
parameter_port.hpp
signal_flow_context.hpp
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

target_link_libraries( ${PROJECT_NAME} visr_impl )
target_link_libraries( ${PROJECT_NAME} ${PYTHON_LIBRARY} )

# ??? Correct? THE LIBRARY_OUTPUT_DIRECTORY should be enough for the compilation.
# Packaging has to be handled specifically.
install(TARGETS ${PROJECT_NAME} DESTINATION python)
