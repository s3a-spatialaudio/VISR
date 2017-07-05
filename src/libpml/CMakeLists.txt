# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
 array_configuration.cpp
 biquad_parameter.cpp
 double_buffering_protocol.cpp
 empty_parameter_config.cpp
 filter_routing_parameter.cpp
 float_sequence.cpp
 index_sequence.cpp
 indexed_value_parameter.cpp
 initialise_parameter_library.cpp
 listener_position.cpp
 matrix_parameter.cpp
 matrix_parameter_config.cpp
 message_queue_protocol.cpp
 object_vector.cpp
 parametric_iir_coefficient.cpp
 scalar_parameter.cpp
 shared_data_protocol.cpp
 signal_routing_parameter.cpp
 string_parameter.cpp
 time_frequency_parameter.cpp
 time_frequency_parameter_config.cpp
 vector_parameter.cpp
 vector_parameter_config.cpp
)

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS
 array_configuration.hpp
 biquad_parameter.hpp
 double_buffering_protocol.hpp
 empty_parameter_config.hpp
 filter_routing_parameter.hpp
 float_sequence.hpp
 index_sequence.hpp
 indexed_value_parameter.hpp
 initialise_parameter_library.hpp
 listener_position.hpp
 matrix_parameter.hpp
 matrix_parameter_config.hpp
 message_queue_protocol.hpp
 object_vector.hpp
 parametric_iir_coefficient.hpp
 scalar_parameter.hpp
 shared_data_protocol.hpp
 signal_routing_parameter.hpp
 string_parameter.hpp
 time_frequency_parameter.hpp
 time_frequency_parameter_config.hpp
 vector_parameter.hpp
 vector_parameter_config.hpp
)

INCLUDE_DIRECTORIES( ${SNDFILE_INCLUDE_DIRS} )

ADD_LIBRARY( pml STATIC ${SOURCES} ${HEADERS} )

target_link_libraries( pml objectmodel )
target_link_libraries( pml ril )
target_link_libraries( pml visr_impl )
target_link_libraries( pml efl )
target_link_libraries( pml ${SNDFILE_LIBRARY} )
TARGET_LINK_LIBRARIES( pml ${Boost_FILESYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES( pml ${Boost_SYSTEM_LIBRARY} )


# Add the unit test directory.
ADD_SUBDIRECTORY( test )
