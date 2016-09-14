# Copyright Institute of Sound and Vibration Research - All rights reserved

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
parameter_connection_descriptor.cpp
parameter_factory.cpp
parameter_port_base.cpp
port_base.cpp
signal_flow_context.cpp
)

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS 
atomic_component.hpp 
audio_connection_descriptor.hpp
audio_input.hpp 
audio_output.hpp 
audio_port.hpp 
communication_protocol_base.hpp 
communication_protocol_type.hpp 
communication_protocol_factory.hpp 
component.hpp 
composite_component.hpp 
constants.hpp 
parameter_base.hpp 
parameter_config_base.hpp 
parameter_connection_descriptor.hpp
parameter_input_port.hpp
parameter_factory.hpp 
parameter_output_port.hpp
parameter_port_base.hpp
parameter_type.hpp
port_base.hpp
processable_interface.hpp
signal_flow_context.hpp
typed_parameter_base.hpp 
)

ADD_LIBRARY( ril STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
