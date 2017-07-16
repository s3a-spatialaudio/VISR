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

TARGET_LINK_LIBRARIES(network_audio_streamer apputilities )
TARGET_LINK_LIBRARIES(network_audio_streamer rcl)
TARGET_LINK_LIBRARIES(network_audio_streamer pml)
TARGET_LINK_LIBRARIES(network_audio_streamer rbbl)
TARGET_LINK_LIBRARIES(network_audio_streamer ril)
TARGET_LINK_LIBRARIES(network_audio_streamer rrl)
TARGET_LINK_LIBRARIES(network_audio_streamer audiointerfaces)
TARGET_LINK_LIBRARIES(network_audio_streamer efl)
TARGET_LINK_LIBRARIES(network_audio_streamer ${PORTAUDIO_LIBRARIES} )
TARGET_LINK_LIBRARIES(network_audio_streamer ${Boost_FILESYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(network_audio_streamer ${Boost_PROGRAM_OPTIONS_LIBRARY} )
TARGET_LINK_LIBRARIES(network_audio_streamer ${Boost_THREAD_LIBRARY} )
TARGET_LINK_LIBRARIES(network_audio_streamer ${Boost_SYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(network_audio_streamer ${CMAKE_THREAD_LIBS_INIT} )

IF( BUILD_USE_JACK )
  TARGET_LINK_LIBRARIES(network_audio_streamer ${JACK_LIBRARY} )
ENDIF( BUILD_USE_JACK )

INSTALL( TARGETS network_audio_streamer DESTINATION bin )
