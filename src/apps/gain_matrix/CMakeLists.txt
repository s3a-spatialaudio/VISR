# Copyright Institute of Sound and Vibration Research - All rights reserved

add_executable( gain_matrix_app main.cpp options.cpp )

target_link_libraries(gain_matrix_app PRIVATE signalflows_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(gain_matrix_app PRIVATE rrl_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(gain_matrix_app PRIVATE audiointerfaces_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(gain_matrix_app PRIVATE apputilities_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(gain_matrix_app PRIVATE visr_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(gain_matrix_app PRIVATE Boost::filesystem )
