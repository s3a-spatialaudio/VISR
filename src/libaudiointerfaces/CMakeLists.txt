# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
audio_interface.cpp
portaudio_interface.cpp
audio_interface_factory.cpp
)

SET( HEADERS
audio_interface.hpp
portaudio_interface.hpp
audio_interface_factory.hpp
)

IF( BUILD_USE_JACK )
  INCLUDE_DIRECTORIES( ${JACK_INCLUDE_DIR} ) 
  LIST( APPEND HEADERS jack_interface.hpp )
  LIST( APPEND SOURCES jack_interface.cpp )
  ADD_DEFINITIONS( "-DVISR_JACK_SUPPORT" )

ENDIF( BUILD_USE_JACK )

INCLUDE_DIRECTORIES( ${PORTAUDIO_INCLUDE_DIR} )

add_library( audiointerfaces_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( audiointerfaces_static PROPERTIES OUTPUT_NAME audiointerfaces )
# Transitive link dependencies
target_link_libraries( audiointerfaces_static rrl_static)
target_link_libraries( audiointerfaces_static ${PORTAUDIO_LIBRARIES} )
if( BUILD_USE_JACK )
  target_link_libraries( audiointerfaces_static ${JACK_LIBRARY} )
  target_link_libraries( audiointerfaces_static rbbl_static )
endif( BUILD_USE_JACK )

add_library( audiointerfaces_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( audiointerfaces_shared PROPERTIES OUTPUT_NAME audiointerfaces )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( audiointerfaces_shared PROPERTIES ARCHIVE_OUTPUT_NAME audiointerfaces_shared )
target_compile_definitions( audiointerfaces_shared PRIVATE -DVISR_BUILD_AUDIOINTERFACES_SHARED_LIBRARY=1 -DVISR_BUILD_AUDIOINTERFACES_LIBRARY=1)
target_compile_definitions( audiointerfaces_shared PUBLIC -DVISR_BUILD_AUDIOINTERFACES_SHARED_LIBRARY=1)
set_target_properties( audiointerfaces_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
target_link_libraries( audiointerfaces_shared rrl_shared )
target_link_libraries( audiointerfaces_shared ${PORTAUDIO_LIBRARIES} )
if( BUILD_USE_JACK )
  target_link_libraries( audiointerfaces_shared ${JACK_LIBRARY} )
  target_link_libraries( audiointerfaces_shared rbbl_shared )
endif( BUILD_USE_JACK )

install(TARGETS audiointerfaces_static ARCHIVE DESTINATION lib )
install(TARGETS audiointerfaces_shared ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib )

# Add the unit test directory.
add_subdirectory( test )
