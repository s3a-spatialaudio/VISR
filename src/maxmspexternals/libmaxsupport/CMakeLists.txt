# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES external_base.cpp)

SET( HEADERS class_registrar.hpp  external_base.hpp external_wrapper.hpp )

INCLUDE_DIRECTORIES( ${MAXSDK_MSP_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${MAXSDK_MAX_INCLUDE_DIR} )

ADD_LIBRARY( maxsupport STATIC ${SOURCES} ${HEADERS} )
