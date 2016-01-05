# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES baseline_renderer.cpp delay_vector.cpp )

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS baseline_renderer.hpp delay_vector.hpp )

ADD_LIBRARY( signalflows STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
