# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
audio_interface.cpp
portaudio_interface.cpp
audio_interface_factory.cpp
)

SET( HEADERS
audio_interface.hpp
portaudio_interface.hpp
audio_interface_factory.hpp
)

IF( BUILD_USE_JACK )
  INCLUDE_DIRECTORIES( ${JACK_INCLUDE_DIR} ) 
  LIST( APPEND HEADERS jack_interface.hpp )
  LIST( APPEND SOURCES jack_interface.cpp )
  ADD_DEFINITIONS( "-DVISR_JACK_SUPPORT" )

ENDIF( BUILD_USE_JACK )

INCLUDE_DIRECTORIES( ${PORTAUDIO_INCLUDE_DIR} )

ADD_LIBRARY( audiointerfaces STATIC ${SOURCES} ${HEADERS} )

# Transitive link dependencies
target_link_libraries( audiointerfaces rrl)


target_link_libraries( audiointerfaces ${PORTAUDIO_LIBRARIES} )
if( BUILD_USE_JACK )
  target_link_libraries( audiointerfaces ${JACK_LIBRARY} )
endif( BUILD_USE_JACK )
target_link_libraries( audiointerfaces pml )
# Add the unit test directory.
add_subdirectory( test )
