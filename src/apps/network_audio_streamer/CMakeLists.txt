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

target_link_libraries(network_audio_streamer PRIVATE apputilities_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(network_audio_streamer PRIVATE rcl_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(network_audio_streamer PRIVATE rrl_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(network_audio_streamer PRIVATE audiointerfaces_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(network_audio_streamer PRIVATE Boost::filesystem )

set_target_properties( network_audio_streamer PROPERTIES FOLDER applications )

install( TARGETS network_audio_streamer
         DESTINATION bin
         COMPONENT standalone_applications )
