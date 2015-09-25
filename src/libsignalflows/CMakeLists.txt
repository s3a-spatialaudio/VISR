# Copyright Institute of Sound and Vibration Research - All rights reserved

# Temporarily pass this path to baseline_renderer.cpp to allow use of hard-coded file path for the initial HOA support.
ADD_DEFINITIONS( -DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" )

SET( SOURCES baseline_renderer.cpp delay_vector.cpp )

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS baseline_renderer.hpp delay_vector.hpp )

ADD_LIBRARY( signalflows STATIC ${SOURCES} ${HEADERS} )
