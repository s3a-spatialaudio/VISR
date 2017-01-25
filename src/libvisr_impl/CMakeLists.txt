# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( HEADERS
audio_connection_descriptor.hpp
composite_component_implementation.hpp
parameter_connection_descriptor.hpp
)

SET( SOURCES 
atomic_component.cpp
audio_connection_descriptor.cpp
audio_input.cpp
audio_output.cpp
audio_port.cpp
communication_protocol_base.cpp
communication_protocol_factory.cpp
component.cpp
composite_component.cpp
composite_component_implementation.cpp
parameter_connection_descriptor.cpp
parameter_factory.cpp
parameter_port_base.cpp
port_base.cpp
signal_flow_context.cpp
)

ADD_LIBRARY( visr_impl STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
