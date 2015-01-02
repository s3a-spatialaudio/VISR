# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES object.cpp object_factory.cpp object_type.cpp object_vector.cpp object_vector_parser.cpp plane_wave.cpp point_source.cpp point_source_parser.cpp )
SET( HEADERS object.hpp object_factory.hpp object_type.hpp object_vector.hpp object_vector_parser.hpp plane_wave.hpp point_source.hpp point_source_parser.hpp )

ADD_LIBRARY( objectmodel STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
