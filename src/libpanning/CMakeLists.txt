# Copyright Institute of Sound and Vibration Research - All rights reserved



SET( SOURCES AllRAD.cpp LoudspeakerArray.cpp VBAP.cpp CAP.cpp XYZ.cpp )

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS AllRAD.h LoudspeakerArray.h VBAP.h CAP.h XYZ.h)

ADD_LIBRARY( panning STATIC ${SOURCES} ${HEADERS} )

# Add the unit tests.
ADD_SUBDIRECTORY( test )