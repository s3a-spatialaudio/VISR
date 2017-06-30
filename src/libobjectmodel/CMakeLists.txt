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

add_library( objectmodel STATIC ${SOURCES} ${HEADERS} )

target_link_libraries( objectmodel rbbl )

add_library( objectmodel_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( objectmodel_shared PROPERTIES OUTPUT_NAME objectmodel )
target_link_libraries( objectmodel_shared rbbl )
target_compile_definitions( objectmodel_shared PRIVATE -DVISR_BUILD_OBJECTMODEL_SHARED_LIBRARY=1 -DVISR_BUILD_OBJECTMODEL_LIBRARY=1)

# Add the unit test directory.
add_subdirectory( test )
