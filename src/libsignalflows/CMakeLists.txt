# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES 
core_renderer.cpp
baseline_renderer.cpp 
bunch_renderer.cpp 
delay_vector.cpp 
gain_matrix.cpp
time_frequency_feedthrough.cpp
)

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS 
baseline_renderer.hpp
bunch_renderer.hpp
core_renderer.hpp
delay_vector.hpp 
gain_matrix.hpp
time_frequency_feedthrough.hpp
)

ADD_LIBRARY( signalflows STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
