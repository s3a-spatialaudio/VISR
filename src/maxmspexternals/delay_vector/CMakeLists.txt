# Copyright Institute of Sound and Vibration Research - All rights reserved

INCLUDE_DIRECTORIES( ${MAXSDK_MSP_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${MAXSDK_MAX_INCLUDE_DIR} )

IF( APPLE )
  FIND_LIBRARY( CORE_SERVICE_FRAMEWORK NAMES CoreServices )
  INCLUDE_DIRECTORIES( ${CORE_SERVICE_FRAMEWORK}/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Headers )
ELSE( APPLE )
  ADD_DEFINITIONS( -DWIN_VERSION )
ENDIF( APPLE )

SET( EXTERNAL_NAME delay_vector )

ADD_LIBRARY( delay_vector_max_external MODULE delay_vector.c )

set_target_properties( delay_vector_max_external  PROPERTIES
    PREFIX ""
    SUFFIX ""
    OUTPUT_NAME ${EXTERNAL_NAME}
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/max"
)

# Note: It is sufficient if we say 'extern "C"' within to revert the effects of the
# C++ linking?
SET_TARGET_PROPERTIES( delay_vector_max_external PROPERTIES LINKER_LANGUAGE C)
SET_PROPERTY( SOURCE delay_vector.c PROPERTY LANGUAGE C)

IF( WIN32 )
  TARGET_LINK_LIBRARIES( delay_vector_max_external ${MAXSDK_CRT_LIBRARY} )
  TARGET_LINK_LIBRARIES( delay_vector_max_external ${MAXSDK_CRT_P_LIBRARY} )
ENDIF( WIN32 )
TARGET_LINK_LIBRARIES( delay_vector_max_external ${MAXSDK_AUDIO_LIBRARY} )
TARGET_LINK_LIBRARIES( delay_vector_max_external ${MAXSDK_API_LIBRARY} )
# According to the current FindMaxSDK.cmake, there is no crt library for apple.
# TODO: Verify and improve FindMaxSDK.cmake
