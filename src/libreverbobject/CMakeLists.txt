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


add_library( reverbobject_static STATIC ${SOURCES} ${HEADERS} )
# Transitive link dependencies
target_link_libraries( reverbobject_static pml_static )
target_link_libraries( reverbobject_static rcl_static )
target_link_libraries( reverbobject_static objectmodel_static )
target_link_libraries( reverbobject_static efl_static )
target_link_libraries( reverbobject_static ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( reverbobject_static ${Boost_SYSTEM_LIBRARY} )

add_library( reverbobject_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( reverbobject_shared PROPERTIES OUTPUT_NAME reverbobject )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( reverbobject_shared PROPERTIES ARCHIVE_OUTPUT_NAME reverbobject_shared )
target_compile_definitions( reverbobject_shared PRIVATE -DVISR_BUILD_REVERBOBJECT_SHARED_LIBRARY=1 -DVISR_BUILD_REVERBOBJECT_LIBRARY=1)
set_target_properties( reverbobject_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
# Transitive link dependencies
target_link_libraries( reverbobject_shared pml_shared )
target_link_libraries( reverbobject_shared rcl_shared )
target_link_libraries( reverbobject_shared objectmodel_shared )
target_link_libraries( reverbobject_shared efl_shared )
target_link_libraries( reverbobject_shared ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( reverbobject_shared ${Boost_SYSTEM_LIBRARY} )

install(TARGETS reverbobject_static ARCHIVE DESTINATION lib )
install(TARGETS reverbobject_shared ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib )

# Add the unit test directory.
# ADD_SUBDIRECTORY( test )
