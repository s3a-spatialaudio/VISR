# Copyright Institute of Sound and Vibration Research - All rights reserved

add_definitions( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

add_executable( baseline_renderer main.cpp options.hpp options.cpp )

target_link_libraries(baseline_renderer apputilities_static )
target_link_libraries(baseline_renderer signalflows_static )
target_link_libraries(baseline_renderer audiointerfaces_static )
target_link_libraries(baseline_renderer rrl_static )
target_link_libraries(baseline_renderer ${CMAKE_THREAD_LIBS_INIT} )

install( TARGETS baseline_renderer DESTINATION bin )
