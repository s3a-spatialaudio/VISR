# Copyright Institute of Sound and Vibration Research - All rights reserved



SET( SOURCES AllRAD.cpp LoudspeakerArray.cpp VBAP.cpp XYZ.cpp )

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS AllRAD.h LoudspeakerArray.h VBAP.h XYZ.h)

ADD_LIBRARY( panning STATIC ${SOURCES} ${HEADERS} )

target_link_libraries( panning rbbl )
target_link_libraries( panning ${Boost_REGEX_LIBRARY} )
target_link_libraries( panning ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( panning ${Boost_SYSTEM_LIBRARY} )

# Add the unit tests.
ADD_SUBDIRECTORY( test )