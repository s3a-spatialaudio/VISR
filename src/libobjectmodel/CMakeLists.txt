# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES audio_object.cpp audio_object_parser.cpp object_factory.cpp object_type.cpp object_vector.cpp plane_wave.cpp point_source.cpp point_source_parser.cpp )
SET( HEADERS audio_object.hpp audio_object_parser.hpp object_factory.hpp object_type.hpp object_vector.hpp plane_wave.hpp point_source.hpp point_source_parser.hpp )

ADD_LIBRARY( objectmodel STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
