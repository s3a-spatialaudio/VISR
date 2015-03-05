# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES diffuse_source.cpp diffuse_source_parser.cpp object.cpp object_factory.cpp object_parser.cpp object_type.cpp object_vector.cpp object_vector_parser.cpp plane_wave.cpp plane_wave_parser.cpp point_source.cpp point_source_parser.cpp point_source_with_diffuseness.cpp point_source_with_diffuseness_parser.cpp)
SET( HEADERS diffuse_source.hpp diffuse_source_parser.hpp object.hpp object_factory.hpp object_parser.hpp object_type.hpp object_vector.hpp object_vector_parser.hpp plane_wave.hpp plane_wave_parser.hpp point_source.hpp point_source_parser.hpp point_source_with_diffuseness.hpp point_source_with_diffuseness_parser.hpp)

ADD_LIBRARY( objectmodel STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
