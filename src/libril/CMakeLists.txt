# Copyright Institute of Sound and Vibration Research - All rights reserved

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS 
atomic_component.hpp 
audio_input.hpp
audio_output.hpp
audio_port_base.hpp
audio_sample_type.hpp
channel_list.hpp
communication_protocol_base.hpp 
communication_protocol_type.hpp 
communication_protocol_factory.hpp 
component.hpp 
composite_component.hpp 
constants.hpp 
export_symbols.hpp
parameter_base.hpp 
parameter_config_base.hpp 
parameter_input_port.hpp
parameter_factory.hpp 
parameter_output_port.hpp
parameter_port_base.hpp
parameter_type.hpp
port_base.hpp
processable_interface.hpp
signal_flow_context.hpp
status_message.hpp
typed_parameter_base.hpp
version.hpp
)

SET( SOURCES
atomic_component.cpp
audio_input.cpp
audio_output.cpp
audio_port_base.cpp
audio_sample_type.cpp
channel_list.cpp
communication_protocol_base.cpp
component.cpp
composite_component.cpp
parameter_factory.cpp
parameter_port_base.cpp
port_base.cpp
signal_flow_context.cpp
version.cpp
)

# Essentially empty library (Decide what to do with that!)
add_library( ril STATIC ${HEADERS} ${SOURCES} )
target_compile_definitions( ril PRIVATE -DVISR_BUILD_CORE_LIBRARIES )

# Transitive link dependencies
target_link_libraries( ril visr_impl )
