# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES portaudio_interface.cpp )

SET( HEADERS portaudio_interface.hpp )

IF( BUILD_USE_JACK )
  INCLUDE_DIRECTORIES( ${JACK_INCLUDE_DIR} ) 
  LIST( APPEND HEADERS jack_interface.hpp )
  LIST( APPEND SOURCES jack_interface.cpp )
ENDIF( BUILD_USE_JACK )

INCLUDE_DIRECTORIES( ${PORTAUDIO_INCLUDE_DIR} )

ADD_LIBRARY( rrl STATIC ${SOURCES} ${HEADERS} )
