# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES audio_object.cpp object_vector.cpp )
SET( HEADERS audio_object.hpp object_vector.hpp )

ADD_LIBRARY( objectmodel STATIC ${SOURCES} ${HEADERS} )
