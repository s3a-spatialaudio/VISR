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
audio_connection_descriptor.cpp
audio_port_base_implementation.cpp
communication_protocol_factory.cpp
component_impl.cpp
composite_component_implementation.cpp
parameter_connection_descriptor.cpp
parameter_port_base_implementation.cpp
port_base_implementation.cpp
)

add_definitions( -DVISR_BUILD_CORE_LIBRARIES=1 )

ADD_LIBRARY( visr_impl STATIC ${SOURCES} ${HEADERS} )

# Mutual dependeny between ril and vir_impl. This might be problematic when one of them is a shared library.
target_link_libraries( visr_impl ril )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
