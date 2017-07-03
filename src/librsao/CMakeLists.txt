# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
late_reverb_filter_calculator.cpp
late_reverb_parameter.cpp
reverb_object_renderer.cpp
reverb_parameter_calculator.cpp
)

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS
export_symbols.hpp
late_reverb_filter_calculator.hpp
late_reverb_parameter.hpp
reverb_object_renderer.hpp
reverb_parameter_calculator.hpp
)

# Preliminary hack. Used only for scene_sender. 
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/3rd/include/oscpkt )


add_library( rsao STATIC ${SOURCES} ${HEADERS} )
# Transitive link dependencies
target_link_libraries( rsao pml )
target_link_libraries( rsao rcl )
target_link_libraries( rsao objectmodel )
target_link_libraries( rsao efl )
target_link_libraries( rsao ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( rsao ${Boost_SYSTEM_LIBRARY} )

add_library( rsao_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( rsao_shared PROPERTIES OUTPUT_NAME rsao )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( rsao_shared PROPERTIES ARCHIVE_OUTPUT_NAME rsao_shared )
target_compile_definitions( rsao_shared PRIVATE -DVISR_BUILD_RSAO_SHARED_LIBRARY=1 -DVISR_BUILD_RSAO_LIBRARY=1)
set_target_properties( rsao_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
# Transitive link dependencies
target_link_libraries( rsao_shared pml_shared )
target_link_libraries( rsao_shared rcl_shared )
target_link_libraries( rsao_shared objectmodel_shared )
target_link_libraries( rsao_shared efl )
target_link_libraries( rsao_shared ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( rsao_shared ${Boost_SYSTEM_LIBRARY} )

# Add the unit test directory.
# ADD_SUBDIRECTORY( test )
