# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES audio_object.cpp object_vector.cpp point_source.cpp )
SET( HEADERS audio_object.hpp object_vector.hpp point_source.hpp )

ADD_LIBRARY( objectmodel STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )