# Copyright Institute of Sound and Vibration Research - All rights reserved

set( SOURCES
 audio_network_encoder.cpp
 main.cpp
 options.cpp
 signal_flow.cpp
)

set( HEADERS
 audio_network_encoder.hpp
 options.hpp
 signal_flow.hpp
)

add_executable( network_audio_streamer ${SOURCES} ${HEADERS} )

target_link_libraries(network_audio_streamer apputilities_shared )
target_link_libraries(network_audio_streamer rcl_shared )
target_link_libraries(network_audio_streamer rrl_shared )
target_link_libraries(network_audio_streamer audiointerfaces_shared )
target_link_libraries(network_audio_streamer Boost::filesystem )

install( TARGETS network_audio_streamer
         DESTINATION bin
         COMPONENT standalone_applications )
