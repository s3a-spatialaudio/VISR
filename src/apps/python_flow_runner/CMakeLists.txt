# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME python_flow_runner )

# Preliminary solution: communicate to main.cpp that Jack support is enabled.
# TODO: Replace by factory mechanism to select audio interface.
IF( BUILD_USE_JACK )
  ADD_DEFINITIONS( "-DVISR_JACK_SUPPORT" )
ENDIF( BUILD_USE_JACK )

INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIR} )

ADD_EXECUTABLE( ${APPLICATION_NAME} main.cpp options.hpp options.cpp )

TARGET_LINK_LIBRARIES(${APPLICATION_NAME} audiointerfaces)
TARGET_LINK_LIBRARIES(${APPLICATION_NAME} apputilities)
TARGET_LINK_LIBRARIES(${APPLICATION_NAME} pythonsupport)
TARGET_LINK_LIBRARIES(${APPLICATION_NAME} rrl)
TARGET_LINK_LIBRARIES(${APPLICATION_NAME} pml)
TARGET_LINK_LIBRARIES(${APPLICATION_NAME} ${CMAKE_THREAD_LIBS_INIT} )
