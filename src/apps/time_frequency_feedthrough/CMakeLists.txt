# Copyright Institute of Sound and Vibration Research - All rights reserved

add_executable( time_frequency_feedthrough main.cpp )

target_link_libraries(time_frequency_feedthrough signalflows_shared )
target_link_libraries(time_frequency_feedthrough rrl_shared )
target_link_libraries(time_frequency_feedthrough audiointerfaces_shared )
