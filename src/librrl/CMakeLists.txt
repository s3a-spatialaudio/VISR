# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
audio_connection_map.cpp
audio_signal_flow.cpp
communication_area.cpp
integrity_checking.cpp
parameter_connection_graph.cpp
parameter_connection_map.cpp
port_utilities.cpp
#portaudio_interface.cpp
scheduling_graph.cpp
signal_routing_internal.cpp
)

SET( HEADERS
audio_connection_map.hpp
audio_signal_flow.hpp
communication_area.hpp
export_symbols.hpp
integrity_checking.hpp
parameter_connection_graph.hpp
parameter_connection_map.hpp
#portaudio_interface.hpp
port_utilities.hpp
scheduling_graph.hpp
signal_routing_internal.hpp
)

#IF( BUILD_USE_JACK )
#  INCLUDE_DIRECTORIES( ${JACK_INCLUDE_DIR} ) 
#  LIST( APPEND HEADERS jack_interface.hpp )
#  LIST( APPEND SOURCES jack_interface.cpp )
#ENDIF( BUILD_USE_JACK )

#INCLUDE_DIRECTORIES( ${PORTAUDIO_INCLUDE_DIR} )

add_library( rrl STATIC ${SOURCES} ${HEADERS} )
# Transitive link dependencies
target_link_libraries( rrl visr_impl )
target_link_libraries( rrl efl )

add_library( rrl_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( rrl_shared PROPERTIES OUTPUT_NAME rrl )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( rrl_shared PROPERTIES ARCHIVE_OUTPUT_NAME rrl_shared )
target_compile_definitions( rrl_shared PRIVATE -DVISR_BUILD_RRL_SHARED_LIBRARY=1 -DVISR_BUILD_RRL_LIBRARY=1)
set_target_properties( rrl_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
# Transitive link dependencies
target_link_libraries( rrl_shared visr_impl_shared )
target_link_libraries( rrl_shared efl_shared )

# Add the unit test directory.
add_subdirectory( test )
