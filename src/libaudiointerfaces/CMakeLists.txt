# Copyright Institute of Sound and Vibration Research - All rights reserved

set( SOURCES
audio_interface.cpp
portaudio_interface.cpp
audio_interface_factory.cpp
)

set( HEADERS
audio_interface.hpp
export_symbols.hpp  
portaudio_interface.hpp
audio_interface_factory.hpp
)

if( BUILD_USE_JACK )
  list( APPEND HEADERS jack_interface.hpp )
  list( APPEND SOURCES jack_interface.cpp )
endif( BUILD_USE_JACK )

add_library( audiointerfaces_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( audiointerfaces_static PROPERTIES OUTPUT_NAME audiointerfaces )
target_compile_definitions( audiointerfaces_static PUBLIC VISR_AUDIOINTERFACES_STATIC_LIBRARY=1 )

add_library( audiointerfaces_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( audiointerfaces_shared PROPERTIES OUTPUT_NAME audiointerfaces )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( audiointerfaces_shared PROPERTIES ARCHIVE_OUTPUT_NAME audiointerfaces_shared )

set_target_properties( audiointerfaces_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )

# Set target properties common to both library types.
foreach(LIB_TYPE static shared)
  target_compile_definitions( audiointerfaces_${LIB_TYPE} PUBLIC VISR_BUILD_AUDIOINTERFACES_LIBRARY=1)

  target_link_libraries( audiointerfaces_${LIB_TYPE} PRIVATE rrl_${LIB_TYPE} )
  target_link_libraries( audiointerfaces_${LIB_TYPE} PRIVATE Boost::boost ) # Set the boost include directory.
  target_link_libraries( audiointerfaces_${LIB_TYPE} PRIVATE ${PORTAUDIO_LIBRARIES} )
  target_include_directories( audiointerfaces_${LIB_TYPE} PRIVATE ${PORTAUDIO_INCLUDE_DIR} )
  if( BUILD_USE_JACK )
    target_include_directories( audiointerfaces_${LIB_TYPE} PRIVATE ${JACK_INCLUDE_DIR} ) 
    target_compile_definitions( audiointerfaces_${LIB_TYPE} PRIVATE -DVISR_JACK_SUPPORT )
    target_link_libraries( audiointerfaces_${LIB_TYPE} PRIVATE ${JACK_LIBRARY} )
    target_link_libraries( audiointerfaces_${LIB_TYPE} PRIVATE rbbl_${LIB_TYPE} )
  endif( BUILD_USE_JACK )
endforeach()

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

install(TARGETS audiointerfaces_static EXPORT VISR_exports
        ARCHIVE DESTINATION lib COMPONENT static_libraries )
install(TARGETS audiointerfaces_shared
        COMPONENT shared_libraries EXPORT VISR_exports
        ARCHIVE DESTINATION lib COMPONENT development_files
        RUNTIME DESTINATION lib COMPONENT shared_libraries
        LIBRARY DESTINATION lib COMPONENT shared_libraries
        PUBLIC_HEADER DESTINATION include/visr/libaudiointerfaces COMPONENT development_files )

# Add the unit test directory.
add_subdirectory( test )
