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

SET( PUBLIC_HEADERS
audio_signal_flow.hpp
export_symbols.hpp
integrity_checking.hpp
)

SET( INTERNAL_HEADERS
audio_connection_map.hpp
export_symbols.hpp
integrity_checking.hpp
)


add_library( rrl_static STATIC ${SOURCES} ${PUBLIC_HEADERS} ${INTERNAL_HEADERS} )
set_target_properties( rrl_static PROPERTIES OUTPUT_NAME rrl )
# Transitive link dependencies
target_link_libraries( rrl_static visr_static )
target_link_libraries( rrl_static efl_static )

add_library( rrl_shared SHARED ${SOURCES} ${PUBLIC_HEADERS} ${INTERNAL_HEADERS} )
set_target_properties( rrl_shared PROPERTIES OUTPUT_NAME rrl )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( rrl_shared PROPERTIES ARCHIVE_OUTPUT_NAME rrl_shared )
target_compile_definitions( rrl_shared PRIVATE -DVISR_BUILD_RRL_SHARED_LIBRARY=1 -DVISR_BUILD_RRL_LIBRARY=1)
target_compile_definitions( rrl_shared PUBLIC -DVISR_BUILD_RRL_SHARED_LIBRARY=1)
# Transitive link dependencies
target_link_libraries( rrl_shared visr_shared )
target_link_libraries( rrl_shared efl_shared )

# Set public headers to be installed for the shared library target.
# Because the headers are the same for the static library, the headers are only
# installed once.
set_target_properties(rrl_shared PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}" )

install(TARGETS rrl_static ARCHIVE DESTINATION lib )
install(TARGETS rrl_shared ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/visr/librrl )

# Add the unit test directory.
add_subdirectory( test )
