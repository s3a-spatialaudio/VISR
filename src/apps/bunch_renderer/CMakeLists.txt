# Copyright Institute of Sound and Vibration Research - All rights reserved

add_definitions( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

add_executable( bunch_renderer main.cpp options.hpp options.cpp )

target_link_libraries(bunch_renderer signalflows_static )
target_link_libraries(bunch_renderer rrl_static )
target_link_libraries(bunch_renderer audiointerfaces_static )
target_link_libraries(bunch_renderer apputilities_static )
