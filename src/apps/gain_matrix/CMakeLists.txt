# Copyright Institute of Sound and Vibration Research - All rights reserved

add_executable( gain_matrix_app main.cpp options.cpp )

target_link_libraries(gain_matrix_app signalflows)
target_link_libraries(gain_matrix_app rrl)
target_link_libraries(gain_matrix_app audiointerfaces)
target_link_libraries(gain_matrix_app apputilities)
