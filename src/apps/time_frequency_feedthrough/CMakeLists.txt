# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_DEFINITIONS( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

ADD_EXECUTABLE( time_frequency_feedthrough main.cpp )

TARGET_LINK_LIBRARIES(time_frequency_feedthrough signalflows)
TARGET_LINK_LIBRARIES(time_frequency_feedthrough rrl)
TARGET_LINK_LIBRARIES(time_frequency_feedthrough audiointerfaces)
TARGET_LINK_LIBRARIES(time_frequency_feedthrough ${CMAKE_THREAD_LIBS_INIT} )
