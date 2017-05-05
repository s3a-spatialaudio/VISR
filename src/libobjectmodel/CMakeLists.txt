# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES 
channel_object.cpp
channel_object_parser.cpp
diffuse_source.cpp
diffuse_source_parser.cpp
hoa_source.cpp
hoa_source_parser.cpp
object.cpp
object_factory.cpp
object_parser.cpp
object_type.cpp
object_vector.cpp
object_vector_parser.cpp
plane_wave.cpp
plane_wave_parser.cpp
point_source.cpp
point_source_extent.cpp
point_source_extent_parser.cpp
point_source_parser.cpp
point_source_with_reverb.cpp
point_source_with_reverb_parser.cpp
point_source_with_diffuseness.cpp
point_source_with_diffuseness_parser.cpp
point_source_with_reverb.cpp
point_source_with_reverb_parser.cpp
)

SET( HEADERS
channel_object.hpp
channel_object_parser.hpp
diffuse_source.hpp
diffuse_source_parser.hpp
hoa_source.hpp
hoa_source_parser.hpp
object.hpp
object_factory.hpp
object_parser.hpp
object_type.hpp
object_vector.hpp
object_vector_parser.hpp
plane_wave.hpp
plane_wave_parser.hpp
point_source.hpp
point_source_extent.hpp
point_source_extent_parser.hpp
point_source_parser.hpp
point_source_with_reverb.hpp
point_source_with_reverb_parser.hpp
point_source_with_diffuseness.hpp
point_source_with_diffuseness_parser.hpp
point_source_with_reverb.hpp
point_source_with_reverb_parser.hpp
)

ADD_LIBRARY( objectmodel STATIC ${SOURCES} ${HEADERS} )

target_link_libraries( objectmodel pml ) # Most likely a circular dependency at the moment
# (critical only shared libraries are involved)
# Incidentally, we are not really using parameter types , but only utility (parsing) functionality from libpml.

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
