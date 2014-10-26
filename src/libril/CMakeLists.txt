# Copyright Institute of Sound and Vibratioin Research - All rights reserved

SET( SOURCES audio_component.cpp audio_input.cpp audio_output.cpp audio_port.cpp audio_signal_flow.cpp communication_area.cpp )

# Basically, this makes the filters show up in the Visual studio project.
SET( HEADERS audio_component.hpp audio_input.hpp audio_output.hpp audio_port.hpp audio_signal_flow.hpp communication_area.hpp )

ADD_LIBRARY( ril STATIC ${SOURCES} ${HEADERS} )
