# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES atomic_component.cpp audio_input.cpp audio_output.cpp audio_port.cpp audio_signal_flow.cpp communication_area.cpp component.cpp composite_component.cpp)

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS atomic_component.hpp audio_input.hpp audio_output.hpp audio_port.hpp audio_signal_flow.hpp communication_area.hpp component.hpp  composite_component.hpp constants.hpp )

ADD_LIBRARY( ril STATIC ${SOURCES} ${HEADERS} )
