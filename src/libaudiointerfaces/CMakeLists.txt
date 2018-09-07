# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
audio_interface.cpp
portaudio_interface.cpp
audio_interface_factory.cpp
)

SET( HEADERS
audio_interface.hpp
export_symbols.hpp  
portaudio_interface.hpp
audio_interface_factory.hpp
)

IF( BUILD_USE_JACK )
  INCLUDE_DIRECTORIES( ${JACK_INCLUDE_DIR} ) 
  LIST( APPEND HEADERS jack_interface.hpp )
  LIST( APPEND SOURCES jack_interface.cpp )
  ADD_DEFINITIONS( "-DVISR_JACK_SUPPORT" )

ENDIF( BUILD_USE_JACK )

add_library( audiointerfaces_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( audiointerfaces_static PROPERTIES OUTPUT_NAME audiointerfaces )
# Transitive link dependencies
target_link_libraries( audiointerfaces_static PRIVATE rrl_static)
target_link_libraries( audiointerfaces_static PRIVATE ${PORTAUDIO_LIBRARIES} )
target_link_libraries( audiointerfaces_static PRIVATE Boost::boost ) # Set the boost include directory.
target_include_directories( audiointerfaces_static PRIVATE ${PORTAUDIO_INCLUDE_DIR} )

if( BUILD_USE_JACK )
  target_link_libraries( audiointerfaces_static PRIVATE ${JACK_LIBRARY} )
  target_link_libraries( audiointerfaces_static PRIVATE rbbl_static )
endif( BUILD_USE_JACK )

add_library( audiointerfaces_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( audiointerfaces_shared PROPERTIES OUTPUT_NAME audiointerfaces )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( audiointerfaces_shared PROPERTIES ARCHIVE_OUTPUT_NAME audiointerfaces_shared )
target_compile_definitions( audiointerfaces_shared PRIVATE -DVISR_BUILD_AUDIOINTERFACES_SHARED_LIBRARY=1 -DVISR_BUILD_AUDIOINTERFACES_LIBRARY=1)
target_compile_definitions( audiointerfaces_shared PUBLIC -DVISR_BUILD_AUDIOINTERFACES_SHARED_LIBRARY=1)
set_target_properties( audiointerfaces_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
target_link_libraries( audiointerfaces_shared PRIVATE rrl_shared )
target_link_libraries( audiointerfaces_shared PRIVATE Boost::boost ) # Set the boost include directory.
target_link_libraries( audiointerfaces_shared PRIVATE ${PORTAUDIO_LIBRARIES} )
target_include_directories( audiointerfaces_shared PRIVATE ${PORTAUDIO_INCLUDE_DIR} )
if( BUILD_USE_JACK )
  target_link_libraries( audiointerfaces_shared PRIVATE ${JACK_LIBRARY} )
  target_link_libraries( audiointerfaces_shared PRIVATE rbbl_shared )
endif( BUILD_USE_JACK )

# Set public headers to be installed for the shared library target.
# Because the headers are the same for the static library, the headers are only
# installed once.
set_target_properties( audiointerfaces_shared PROPERTIES PUBLIC_HEADER "${HEADERS}" )

# Set include paths for dependent projects
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
foreach(TARGET_TYPE static shared)
  target_include_directories(audiointerfaces_${TARGET_TYPE}
    PUBLIC
    $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/visr>
  )
endforeach()

install(TARGETS audiointerfaces_static EXPORT VISR_exports ARCHIVE DESTINATION lib )
install(TARGETS audiointerfaces_shared EXPORT VISR_exports ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib  PUBLIC_HEADER DESTINATION include/visr/libaudiointerfaces )

# Add the unit test directory.
add_subdirectory( test )
