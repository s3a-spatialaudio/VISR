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

ADD_DEFINITIONS( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

# Preliminary solution: communicate to main.cpp that Jack support is enabled.
# TODO: Replace by factory mechanism to select audio interface.
IF( BUILD_USE_JACK )
  ADD_DEFINITIONS( "-DVISR_JACK_SUPPORT" )
ENDIF( BUILD_USE_JACK )

ADD_EXECUTABLE( network_audio_streamer ${SOURCES} ${HEADERS} )

TARGET_LINK_LIBRARIES(network_audio_streamer apputilities_static )
TARGET_LINK_LIBRARIES(network_audio_streamer rcl_static )
TARGET_LINK_LIBRARIES(network_audio_streamer rrl_static )
TARGET_LINK_LIBRARIES(network_audio_streamer audiointerfaces_static )
TARGET_LINK_LIBRARIES(network_audio_streamer ${CMAKE_THREAD_LIBS_INIT} )

INSTALL( TARGETS network_audio_streamer
         DESTINATION bin
         COMPONENT standalone_applications )
