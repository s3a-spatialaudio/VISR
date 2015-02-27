# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES iosono_osc_parser.cpp )

SET( HEADERS iosono_osc_parser.hpp )

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/3rd/oscpkt/include )

ADD_LIBRARY( iosonoconvert STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
# ADD_SUBDIRECTORY( test )
