# Copyright Institute of Sound and Vibratioin Research - All rights reserved

# Top-level build configuration for project VISceRal

Project( VISR C )

ADD_LIBRARY( libefl STATIC src/libefl/vector_functions.cpp )

ADD_EXECUTABLE( vbap_renderer src/apps/vbap_renderer/main.cpp )



