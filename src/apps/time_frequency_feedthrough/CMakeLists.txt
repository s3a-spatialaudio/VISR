# Copyright Institute of Sound and Vibration Research - All rights reserved

add_executable( time_frequency_feedthrough main.cpp )

target_link_libraries(time_frequency_feedthrough signalflows_static )
target_link_libraries(time_frequency_feedthrough rrl_static )
target_link_libraries(time_frequency_feedthrough audiointerfaces_static )
target_link_libraries(time_frequency_feedthrough ${CMAKE_THREAD_LIBS_INIT} )
