# Copyright Institute of Sound and Vibratioin Research - All rights reserved

INCLUDE_DIRECTORIES( "($PROJECT_SOURCE_DIR)/libril" )

ADD_LIBRARY( rrl STATIC portaudio_interface.cpp )
