# Copyright Institute of Sound and Vibration Research - All rights reserved

INCLUDE_DIRECTORIES( ${MAXSDK_MSP_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${MAXSDK_MAX_INCLUDE_DIR} )

# Copyright Institute of Sound and Vibration Research - All rights reserved

INCLUDE_DIRECTORIES( ${MAXSDK_MSP_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${MAXSDK_MAX_INCLUDE_DIR} )

IF( APPLE )
  FIND_LIBRARY( CORE_SERVICE_FRAMEWORK NAMES CoreServices )
  INCLUDE_DIRECTORIES( ${CORE_SERVICE_FRAMEWORK}/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Headers )
ELSE( APPLE )
  ADD_DEFINITIONS( -DWIN_VERSION )
ENDIF( APPLE )

SET( EXTERNAL_NAME convert_input )

SET( PROJECT_NAME convert_input_max_external )

ADD_LIBRARY( ${PROJECT_NAME} MODULE convert_input.c )

SET_TARGET_PROPERTIES( ${PROJECT_NAME}  PROPERTIES
    PREFIX ""
    OUTPUT_NAME ${EXTERNAL_NAME}
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/max" )

# <AF> TODO: Move this conditional logic to a centralised location!
if(WIN32) # 32-bit and 64-bit Windows
  if (CMAKE_CL_64)
      SET_TARGET_PROPERTIES( ${PROJECT_NAME} PROPERTIES SUFFIX ~.mxe64)
  else(CMAKE_CL_64)
      SET_TARGET_PROPERTIES( ${PROJECT_NAME} PROPERTIES SUFFIX ~.mxe)
  endif(CMAKE_CL_64)
elseif(APPLE)
  # Force generation of MacOS bundles.
  set_target_properties( ${PROJECT_NAME}  PROPERTIES BUNDLE True )
  set_target_properties( ${PROJECT_NAME}  PROPERTIES BUNDLE_EXTENSION "mxo" )
  set_target_properties( ${PROJECT_NAME}  PROPERTIES XCODE_ATTRIBUTE_MACH_O_TYPE mh_bundle )
  # Workaround for CMake bug https://gitlab.kitware.com/cmake/cmake/issues/16147
  # "BUNDLE_EXTENSION doesn't apply for Xcode generator"
  set_target_properties( ${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_WRAPPER_EXTENSION "mxo")
endif(WIN32)

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} maxsupport signalflows rcl visr_impl rbbl efl )
IF( WIN32 )
  TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${MAXSDK_CRT_LIBRARY} )
  # I need to exclude this library in combination with MAXAPI_USE_MSCRT
  # defined in the source file in order to avoid crashes when C++
  # standard library functions are called.  
  # TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${MAXSDK_CRT_P_LIBRARY} )
  # Still required?
  SET_TARGET_PROPERTIES( ${PROJECT_NAME} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:MSVCRTD.lib" )
ENDIF( WIN32 )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${MAXSDK_AUDIO_LIBRARY} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${MAXSDK_API_LIBRARY} )

# According to the current FindMaxSDK.cmake, there is no crt library for apple.
# TODO: Verify and improve FindMaxSDK.cmake
