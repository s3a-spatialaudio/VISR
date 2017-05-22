# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
portaudio_interface.cpp
)

SET( HEADERS
portaudio_interface.hpp
)

IF( BUILD_USE_JACK )
  INCLUDE_DIRECTORIES( ${JACK_INCLUDE_DIR} ) 
  LIST( APPEND HEADERS jack_interface.hpp )
  LIST( APPEND SOURCES jack_interface.cpp )
ENDIF( BUILD_USE_JACK )

INCLUDE_DIRECTORIES( ${PORTAUDIO_INCLUDE_DIR} )

ADD_LIBRARY( audiointerfaces STATIC ${SOURCES} ${HEADERS} )

# Transitive link dependencies
target_link_libraries( audiointerfaces rrl)


#target_link_libraries( rrl ril )
#target_link_libraries( rrl visr_impl ) # implicitly contained in ril dependency.
#target_link_libraries( rrl efl )
target_link_libraries( audiointerfaces ${PORTAUDIO_LIBRARIES} )
if( BUILD_USE_JACK )
  target_link_libraries( audiointerfaces ${JACK_LIBRARY} )
endif( BUILD_USE_JACK )

# Add the unit test directory.
#add_subdirectory( test )
