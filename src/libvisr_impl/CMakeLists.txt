# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( HEADERS
audio_connection_descriptor.hpp
audio_port_base_implementation.hpp
component_impl.hpp
composite_component_implementation.hpp
parameter_connection_descriptor.hpp
parameter_port_base_implementation.hpp
port_base_implementation.hpp
)

SET( SOURCES 
atomic_component.cpp
audio_connection_descriptor.cpp
audio_input.cpp
audio_output.cpp
audio_port_base.cpp
audio_port_base_implementation.cpp
audio_sample_type.cpp
channel_list.cpp
communication_protocol_base.cpp
communication_protocol_factory.cpp
component.cpp
component_impl.cpp
composite_component.cpp
composite_component_implementation.cpp
parameter_connection_descriptor.cpp
parameter_factory.cpp
parameter_port_base.cpp
parameter_port_base_implementation.cpp
port_base.cpp
port_base_implementation.cpp
signal_flow_context.cpp
)

add_definitions( -DVISR_BUILD_CORE_LIBRARIES=1 )

ADD_LIBRARY( visr_impl STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
