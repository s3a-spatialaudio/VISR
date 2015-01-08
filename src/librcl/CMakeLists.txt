# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES add.cpp gain_matrix.cpp scene_decoder.cpp udp_receiver.cpp)

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS add.hpp gain_matrix.hpp scene_decoder.hpp udp_receiver.hpp)

ADD_LIBRARY( rcl STATIC ${SOURCES} ${HEADERS} )
