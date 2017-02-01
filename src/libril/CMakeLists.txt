# Copyright Institute of Sound and Vibration Research - All rights reserved

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS 
atomic_component.hpp 
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

# Essentially empty library (Decide what to do with that!)
add_library( ril INTERFACE )
# target_include_directories( ril INTERFACE ...)
# target_sources( ril INTERFACE ${HEADERS} ) # requires CMake 3.1 or later

# Since the library is empty, we have to set the language explicitly.
# set_target_properties( ril PROPERTIES LINKER_LANGUAGE CXX)
