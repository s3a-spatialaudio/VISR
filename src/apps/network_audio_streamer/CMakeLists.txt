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
# Preliminary solution: communicate to main.cpp that Jack support is enabled.
# TODO: Replace by factory mechanism to select audio interface.
if( BUILD_USE_JACK )
  target_compile_definitions( network_audio_streamer PRIVATE VISR_JACK_SUPPORT )
endif( BUILD_USE_JACK )

target_link_libraries(network_audio_streamer apputilities_static )
target_link_libraries(network_audio_streamer rcl_static )
target_link_libraries(network_audio_streamer rrl_static )
target_link_libraries(network_audio_streamer audiointerfaces_static )
target_link_libraries(network_audio_streamer ${CMAKE_THREAD_LIBS_INIT} )

install( TARGETS network_audio_streamer DESTINATION bin )
