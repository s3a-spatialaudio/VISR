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
target_link_libraries( reverbobject_static PUBLIC pml_static )
target_link_libraries( reverbobject_static PUBLIC rcl_static )
target_link_libraries( reverbobject_static PUBLIC objectmodel_static )
target_link_libraries( reverbobject_static PUBLIC efl_static )
target_link_libraries( reverbobject_static PRIVATE Boost::filesystem )

add_library( reverbobject_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( reverbobject_shared PROPERTIES OUTPUT_NAME reverbobject )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( reverbobject_shared PROPERTIES ARCHIVE_OUTPUT_NAME reverbobject_shared )
target_compile_definitions( reverbobject_shared PRIVATE -DVISR_BUILD_REVERBOBJECT_SHARED_LIBRARY=1 -DVISR_BUILD_REVERBOBJECT_LIBRARY=1)
set_target_properties( reverbobject_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
# Transitive link dependencies
target_link_libraries( reverbobject_shared PUBLIC pml_shared )
target_link_libraries( reverbobject_shared PUBLIC rcl_shared )
target_link_libraries( reverbobject_shared PUBLIC objectmodel_shared )
target_link_libraries( reverbobject_shared PUBLIC efl_shared )
target_link_libraries( reverbobject_shared PRIVATE Boost::filesystem )

# Set public headers to be installed for the shared target.
# Because the headers are the same for the static library, the headers are only
# installled once.
set_target_properties( reverbobject_shared PROPERTIES PUBLIC_HEADER "${HEADERS}" )

# Set include paths for dependent projects
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
foreach(TARGET_TYPE static shared)
  target_include_directories(reverbobject_${TARGET_TYPE}
    PUBLIC
    $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/visr>
  )
endforeach()

install(TARGETS reverbobject_static EXPORT VISR_exports ARCHIVE DESTINATION lib )
install(TARGETS reverbobject_shared EXPORT VISR_exports ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib  PUBLIC_HEADER DESTINATION include/visr/libreverbobject )

# Add the unit test directory.
# ADD_SUBDIRECTORY( test )
