# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_DEFINITIONS( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

ADD_EXECUTABLE( time_frequency_feedthrough main.cpp )

TARGET_LINK_LIBRARIES(time_frequency_feedthrough signalflows)
TARGET_LINK_LIBRARIES(time_frequency_feedthrough rcl)
TARGET_LINK_LIBRARIES(time_frequency_feedthrough panning)
TARGET_LINK_LIBRARIES(time_frequency_feedthrough rbbl)
TARGET_LINK_LIBRARIES(time_frequency_feedthrough rrl)
TARGET_LINK_LIBRARIES(time_frequency_feedthrough visr_impl)
TARGET_LINK_LIBRARIES(time_frequency_feedthrough pml)
TARGET_LINK_LIBRARIES(time_frequency_feedthrough objectmodel)
TARGET_LINK_LIBRARIES(time_frequency_feedthrough efl)
TARGET_LINK_LIBRARIES(time_frequency_feedthrough ${Boost_FILESYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(time_frequency_feedthrough ${Boost_SYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(time_frequency_feedthrough ${Boost_THREAD_LIBRARY} )
TARGET_LINK_LIBRARIES(time_frequency_feedthrough ${Boost_PROGRAM_OPTIONS_LIBRARY} )
TARGET_LINK_LIBRARIES(time_frequency_feedthrough ${CMAKE_THREAD_LIBS_INIT} )
