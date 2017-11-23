# Copyright Institute of Sound and Vibration Research - All rights reserved

set( SOURCES
 array_configuration.cpp
 biquad_parameter.cpp
 double_buffering_protocol.cpp
 empty_parameter_config.cpp
 filter_routing_parameter.cpp
 indexed_value_parameter.cpp
 initialise_parameter_library.cpp
 interpolation_parameter.cpp
 listener_position.cpp
 matrix_parameter.cpp
 matrix_parameter_config.cpp
 message_queue_protocol.cpp
 object_vector.cpp
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
set( HEADERS
 array_configuration.hpp
 biquad_parameter.hpp
 double_buffering_protocol.hpp
 empty_parameter_config.hpp
 export_symbols.hpp
 filter_routing_parameter.hpp
 indexed_value_parameter.hpp
 initialise_parameter_library.hpp
 interpolation_parameter.hpp
 listener_position.hpp
 matrix_parameter.hpp
 matrix_parameter_config.hpp
 message_queue_protocol.hpp
 object_vector.hpp
 scalar_parameter.hpp
 shared_data_protocol.hpp
 signal_routing_parameter.hpp
 string_parameter.hpp
 time_frequency_parameter.hpp
 time_frequency_parameter_config.hpp
 vector_parameter.hpp
 vector_parameter_config.hpp
)

include_directories( ${SNDFILE_INCLUDE_DIRS} )

add_library( pml_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( pml_static PROPERTIES OUTPUT_NAME pml )
target_link_libraries( pml_static objectmodel_static )
target_link_libraries( pml_static visr_static )
target_link_libraries( pml_static ${SNDFILE_LIBRARY} )
target_link_libraries( pml_static ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( pml_static ${Boost_SYSTEM_LIBRARY} )

add_library( pml_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( pml_shared PROPERTIES OUTPUT_NAME pml )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( pml_shared PROPERTIES ARCHIVE_OUTPUT_NAME pml_shared )
target_compile_definitions( pml_shared PRIVATE -DVISR_BUILD_PML_SHARED_LIBRARY=1 -DVISR_BUILD_PML_LIBRARY=1)
target_compile_definitions( pml_shared PUBLIC -DVISR_BUILD_PML_SHARED_LIBRARY=1)
set_target_properties( pml_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
target_link_libraries( pml_shared objectmodel_shared )
target_link_libraries( pml_shared visr_shared )
target_link_libraries( pml_shared ${SNDFILE_LIBRARY} )
target_link_libraries( pml_shared ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( pml_shared ${Boost_SYSTEM_LIBRARY} )

# Set public headers to be installed for the shared library target.
# Because the headers are the same for the static library, the headers are only
# installed once.
set_target_properties(pml_shared PROPERTIES PUBLIC_HEADER "${HEADERS}" )

install(TARGETS pml_static ARCHIVE DESTINATION lib )
install(TARGETS pml_shared ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/visr/libpml )

# Add the unit test directory.
add_subdirectory( test )
