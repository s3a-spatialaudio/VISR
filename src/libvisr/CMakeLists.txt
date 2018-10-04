# Copyright Institute of Sound and Vibration Research - All rights reserved

# Define the header files that should be installed to the directory include/visr/visr.
SET( PUBLIC_HEADERS 
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
parameter_input.hpp
parameter_factory.hpp 
parameter_output.hpp
parameter_port_base.hpp
parameter_type.hpp
polymorphic_parameter_input.hpp
polymorphic_parameter_output.hpp
port_base.hpp
signal_flow_context.hpp
status_message.hpp
typed_parameter_base.hpp
version.hpp
)

SET( PUBLIC_HEADERS_DETAIL
detail/compile_time_hash_fnv1.hpp
detail/compose_message_string.hpp
)

# Header files used internally, not to be installed.
set( INTERNAL_HEADERS
impl/audio_connection_descriptor.hpp
impl/component_implementation.hpp
impl/composite_component_implementation.hpp
impl/parameter_connection_descriptor.hpp
impl/port_base_implementation.hpp
impl/audio_port_base_implementation.hpp
impl/parameter_port_base_implementation.hpp
)

set( SOURCES
atomic_component.cpp
audio_input.cpp
audio_output.cpp
audio_port_base.cpp
audio_sample_type.cpp
channel_list.cpp
communication_protocol_base.cpp
communication_protocol_factory.cpp
component.cpp
composite_component.cpp
parameter_config_base.cpp
parameter_factory.cpp
parameter_input.cpp
parameter_output.cpp
parameter_base.cpp
parameter_port_base.cpp
polymorphic_parameter_input.cpp
polymorphic_parameter_output.cpp
port_base.cpp
signal_flow_context.cpp
version.cpp
impl/audio_connection_descriptor.cpp
impl/audio_port_base_implementation.cpp
impl/component_implementation.cpp
impl/composite_component_implementation.cpp
impl/parameter_connection_descriptor.cpp
impl/parameter_port_base_implementation.cpp
impl/port_base_implementation.cpp
)

add_library( visr_static STATIC ${SOURCES} ${PUBLIC_HEADERS} ${PUBLIC_HEADERS_DETAIL} ${INTERNAL_HEADERS} )
set_target_properties( visr_static PROPERTIES OUTPUT_NAME visr)
set_target_properties(visr_static PROPERTIES INSTALL_NAME_DIR ${CMAKE_INSTALL_PREFIX}/lib)

add_library( visr_shared SHARED ${SOURCES} ${PUBLIC_HEADERS} ${PUBLIC_HEADERS_DETAIL} ${INTERNAL_HEADERS} )
set_target_properties( visr_shared PROPERTIES OUTPUT_NAME visr)
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( visr_shared PROPERTIES ARCHIVE_OUTPUT_NAME visr_shared )
# Specify that this library is built as a dynamic library.
# 'PUBLIC' means that the definition is also activated for other targets that depend on this library.
target_compile_definitions( visr_shared PUBLIC -DVISR_BUILD_VISR_SHARED_LIBRARY=1 )
# Set this definition only when building the present target.
target_compile_definitions( visr_shared PRIVATE -DVISR_BUILD_VISR_SHARED_LIBRARY=1 -DVISR_BUILD_VISR_LIBRARY=1 )
set_target_properties(visr_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )


# Set public headers to be installed for the shared library target.
# Because the headers are the same for the static library, the headers are only
# installed once.
set_target_properties(visr_shared PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}" )


get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
foreach(TARGET_TYPE static shared)
target_include_directories(
  visr_${TARGET_TYPE}
  PUBLIC
  # this will make impl headers visible when linking to build tree
  # Alternative is to copy public headers to binary dir
  # It's nice to be able to edit headers from dependent projects
  # when doing dev (i.e. not working from an installation) so lesser of two evils?
  $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/visr>)
endforeach()

install(TARGETS visr_static
        EXPORT VISR_exports
        ARCHIVE DESTINATION lib COMPONENT static_libraries )
install(TARGETS visr_shared
        EXPORT VISR_exports
        ARCHIVE DESTINATION lib COMPONENT development_files
        RUNTIME DESTINATION lib COMPONENT shared_libraries
        LIBRARY DESTINATION lib COMPONENT shared_libraries
        PUBLIC_HEADER DESTINATION include/visr/libvisr COMPONENT development_files )
install( FILES ${PUBLIC_HEADERS_DETAIL} DESTINATION include/visr/libvisr/detail COMPONENT development_files )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
