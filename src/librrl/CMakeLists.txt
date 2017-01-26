# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
audio_connection_map.cpp
audio_signal_flow.cpp
communication_area.cpp
parameter_connection_graph.cpp
parameter_connection_map.cpp
port_utilities.cpp
portaudio_interface.cpp
scheduling_graph.cpp
)

SET( HEADERS
audio_connection_map.hpp
audio_interface.hpp
audio_signal_flow.hpp
communication_area.hpp
parameter_connection_graph.hpp
parameter_connection_map.hpp
portaudio_interface.hpp
port_utilities.hpp
scheduling_graph.hpp
)

IF( BUILD_USE_JACK )
  INCLUDE_DIRECTORIES( ${JACK_INCLUDE_DIR} ) 
  LIST( APPEND HEADERS jack_interface.hpp )
  LIST( APPEND SOURCES jack_interface.cpp )
ENDIF( BUILD_USE_JACK )

INCLUDE_DIRECTORIES( ${PORTAUDIO_INCLUDE_DIR} )

ADD_LIBRARY( rrl STATIC ${SOURCES} ${HEADERS} )

target_link_libraries( rrl ${PORTAUDIO_LIBRARIES} )
if( BUILD_USE_JACK )
  target_link_libraries( rrl ${JACK_LIBRARY} )
endif( BUILD_USE_JACK )

# Add the unit test directory.
add_subdirectory( test )
