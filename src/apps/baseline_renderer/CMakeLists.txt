# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_DEFINITIONS( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

ADD_EXECUTABLE( baseline_renderer main.cpp options.hpp options.cpp )

TARGET_LINK_LIBRARIES(baseline_renderer apputilities )
TARGET_LINK_LIBRARIES(baseline_renderer signalflows)
TARGET_LINK_LIBRARIES(baseline_renderer audiointerfaces)
TARGET_LINK_LIBRARIES(baseline_renderer rrl)

INSTALL( TARGETS baseline_renderer DESTINATION bin )
TARGET_LINK_LIBRARIES(baseline_renderer ${CMAKE_THREAD_LIBS_INIT} )
