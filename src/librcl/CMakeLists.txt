# Copyright Institute of Sound and Vibratioin Research - All rights reserved

INCLUDE_DIRECTORIES( "($PROJECT_SOURCE_DIR)/libefl" )

SET( SOURCES add.cpp )

# Basically, this makes the filters show up in the Visual studio project.
SET( HEADERS add.hpp )

ADD_LIBRARY( rcl STATIC ${SOURCES} ${HEADERS} )
