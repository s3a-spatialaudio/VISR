# Copyright Institute of Sound and Vibration Research - All rights reserved

INCLUDE_DIRECTORIES( ${MAXSDK_MSP_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${MAXSDK_MAX_INCLUDE_DIR} )

ADD_DEFINITIONS( -DWIN_VERSION )

SET( EXTERNAL_NAME convert_input )

ADD_LIBRARY( convert_input MODULE convert_input.c )

# Note: It is sufficient if we say 'extern "C"' within to revert the effects of the
# C++ linking?
SET_TARGET_PROPERTIES( convert_input PROPERTIES LINKER_LANGUAGE C)
SET_PROPERTY( SOURCE convert_input.c PROPERTY LANGUAGE C)

IF( WIN32 )
  TARGET_LINK_LIBRARIES( convert_input ${MAXSDK_CRT_LIBRARY} )
  TARGET_LINK_LIBRARIES( convert_input ${MAXSDK_CRT_P_LIBRARY} )
ENDIF( WIN32 )
TARGET_LINK_LIBRARIES( convert_input ${MAXSDK_AUDIO_LIBRARY} )
TARGET_LINK_LIBRARIES( convert_input ${MAXSDK_API_LIBRARY} )
# According to the current FindMaxSDK.cmake, there is no crt library for apple.
# TODO: Verify and improve FindMaxSDK.cmake
