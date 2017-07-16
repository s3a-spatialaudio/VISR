# Copyright Institute of Sound and Vibration Research - All rights reserved

IF( APPLE )
  FIND_LIBRARY( CORE_SERVICE_FRAMEWORK NAMES CoreServices )
  INCLUDE_DIRECTORIES( ${CORE_SERVICE_FRAMEWORK}/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Headers )
ENDIF( APPLE )

SET( SOURCES
argument_list.cpp
external_base.cpp
signal_flow_wrapper.cpp
)

SET( HEADERS
argument_list.hpp
class_registrar.hpp
external_base.hpp
external_wrapper.hpp
signal_flow_wrapper.hpp)

INCLUDE_DIRECTORIES( ${MAXSDK_MSP_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${MAXSDK_MAX_INCLUDE_DIR} )

ADD_LIBRARY( maxsupport STATIC ${SOURCES} ${HEADERS} )

# Transitive dependencies
target_link_libraries( maxsupport rrl )
target_link_libraries( maxsupport audiointerfaces )
