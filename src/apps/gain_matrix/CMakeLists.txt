# Copyright Institute of Sound and Vibration Research - All rights reserved

add_executable( gain_matrix_app main.cpp options.cpp )

target_link_libraries(gain_matrix_app signalflows_shared )
target_link_libraries(gain_matrix_app rrl_shared )
target_link_libraries(gain_matrix_app audiointerfaces_shared )
target_link_libraries(gain_matrix_app apputilities_shared )
target_link_libraries(gain_matrix_app visr_shared )
target_link_libraries(gain_matrix_app Boost::filesystem )
