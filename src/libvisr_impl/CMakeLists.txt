# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( HEADERS
audio_connection_descriptor.hpp
audio_port_base_implementation.hpp
component_implementation.hpp
composite_component_implementation.hpp
parameter_connection_descriptor.hpp
parameter_port_base_implementation.hpp
polymorphic_parameter_input.hpp
polymorphic_parameter_output.hpp
port_base_implementation.hpp
)

SET( SOURCES 
audio_connection_descriptor.cpp
audio_port_base_implementation.cpp
communication_protocol_factory.cpp
component_implementation.cpp
composite_component_implementation.cpp
parameter_base.cpp
parameter_connection_descriptor.cpp
parameter_port_base_implementation.cpp
polymorphic_parameter_input.cpp
polymorphic_parameter_output.cpp
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

add_library( visr_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( visr_static PROPERTIES OUTPUT_NAME visr)
set_target_properties(visr_static PROPERTIES INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib)
# Mutual dependency between ril and vir_impl.
target_link_libraries( visr_static ril )

# Collection of object files compiled as position-independent code, to be used
# in shared libraries and loadable modules
# The separate OBJECT library avoids a third set of objects to be compiled. 
add_library( visr_position_independent_objects OBJECT ${SOURCES} ${HEADERS} )
set_target_properties( visr_position_independent_objects PROPERTIES POSITION_INDEPENDENT_CODE True )
# Specify that this library is built as a dynamic library.
# 'PUBLIC' means that the definition is also activated for other targets that depend on this library.
target_compile_definitions( visr_position_independent_objects PUBLIC -DVISR_BUILD_VISR_SHARED_LIBRARY=1 )
# Set this definition only when building the present target.
target_compile_definitions( visr_position_independent_objects PRIVATE -DVISR_BUILD_VISR_LIBRARY=1 )

add_library( visr_shared SHARED $<TARGET_OBJECTS:visr_position_independent_objects>  )
set_target_properties( visr_shared PROPERTIES OUTPUT_NAME visr)
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( visr_shared PROPERTIES ARCHIVE_OUTPUT_NAME visr_shared )
set_target_properties(visr_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
set_target_properties(visr_shared PROPERTIES INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib)

# Mutual dependency between ril and vir_impl. This might be problematic when one of them is a shared library.
target_link_libraries( visr_shared ril )

# Additional static library that contains position-dependent object code
# These are use for statically linked loadable modules (e.g., DAW plugins or Max/MSP externals)
add_library( visr_position_independent_static STATIC $<TARGET_OBJECTS:visr_position_independent_objects>  )
set_target_properties( visr_position_independent_static PROPERTIES OUTPUT_NAME visr_position_independent_static )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )

install(TARGETS visr_static DESTINATION lib )
