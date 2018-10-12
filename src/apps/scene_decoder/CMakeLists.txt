# Copyright Institute of Sound and Vibration Research - All rights reserved

add_definitions( -DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" )

add_executable( scene_decoder_app main.cpp signal_flow.cpp signal_flow.hpp )

target_link_libraries(scene_decoder_app rcl_shared )
target_link_libraries(scene_decoder_app rrl_shared )
target_link_libraries(scene_decoder_app audiointerfaces_shared )
target_link_libraries(scene_decoder_app Boost::filesystem )
