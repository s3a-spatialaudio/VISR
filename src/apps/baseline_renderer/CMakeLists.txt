# Copyright Institute of Sound and Vibration Research - All rights reserved

add_definitions( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

add_executable( baseline_renderer main.cpp options.hpp options.cpp )

target_link_libraries(baseline_renderer apputilities_shared )
target_link_libraries(baseline_renderer signalflows_shared )
target_link_libraries(baseline_renderer audiointerfaces_shared )
target_link_libraries(baseline_renderer rrl_shared )
target_link_libraries(baseline_renderer Boost::filesystem )

install( TARGETS baseline_renderer DESTINATION bin COMPONENT standalone_applications )
