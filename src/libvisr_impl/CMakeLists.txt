# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( HEADERS
audio_connection_descriptor.hpp
audio_port_base_implementation.hpp
component_implementation.hpp
composite_component_implementation.hpp
parameter_connection_descriptor.hpp
parameter_port_base_implementation.hpp
polymorphic_parameter_input.hpp
port_base_implementation.hpp
)

SET( SOURCES 
audio_connection_descriptor.cpp
audio_port_base_implementation.cpp
communication_protocol_factory.cpp
component_implementation.cpp
composite_component_implementation.cpp
parameter_connection_descriptor.cpp
parameter_port_base_implementation.cpp
polymorphic_parameter_input.cpp
port_base_implementation.cpp
atomic_component.cpp
audio_input.cpp
audio_output.cpp
audio_port_base.cpp
audio_sample_type.cpp
channel_list.cpp
communication_protocol_base.cpp
component.cpp
composite_component.cpp
parameter_config_base.cpp
parameter_factory.cpp
parameter_input.cpp
parameter_output.cpp
parameter_port_base.cpp
port_base.cpp
signal_flow_context.cpp
)

add_definitions( -DVISR_BUILD_CORE_LIBRARIES=1 )

ADD_LIBRARY( visr_impl SHARED ${SOURCES} ${HEADERS} )
set_target_properties(visr_impl PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/shlib" )

# Mutual dependeny between ril and vir_impl. This might be problematic when one of them is a shared library.
target_link_libraries( visr_impl ril )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
