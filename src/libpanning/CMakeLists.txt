# Copyright Institute of Sound and Vibration Research - All rights reserved



SET( SOURCES
 AllRAD.cpp
 LoudspeakerArray.cpp
 VBAP.cpp
 XYZ.cpp
)

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS
 defs.h
 export_symbols.hpp
 AllRAD.h
 LoudspeakerArray.h
 VBAP.h
 XYZ.h
)

add_library( panning STATIC ${SOURCES} ${HEADERS} )
target_link_libraries( panning rbbl )
target_link_libraries( panning ${Boost_REGEX_LIBRARY} )
target_link_libraries( panning ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( panning ${Boost_SYSTEM_LIBRARY} )

add_library( panning_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( panning_shared PROPERTIES OUTPUT_NAME panning )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( panning_shared PROPERTIES ARCHIVE_OUTPUT_NAME panning_shared )
target_compile_definitions( panning_shared PRIVATE -DVISR_BUILD_PANNING_SHARED_LIBRARY=1 -DVISR_BUILD_PANNING_LIBRARY=1)
set_target_properties( panning_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
target_link_libraries( panning_shared rbbl )
target_link_libraries( panning_shared ${Boost_REGEX_LIBRARY} )
target_link_libraries( panning_shared ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( panning_shared ${Boost_SYSTEM_LIBRARY} )

# Add the unit tests.
ADD_SUBDIRECTORY( test )
