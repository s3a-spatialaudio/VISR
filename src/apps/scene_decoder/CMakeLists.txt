# Copyright Institute of Sound and Vibration Research - All rights reserved

add_definitions( -DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" )

add_executable( scene_decoder_app main.cpp signal_flow.cpp signal_flow.hpp )

target_link_libraries(scene_decoder_app rcl_static )
target_link_libraries(scene_decoder_app rrl_static )
target_link_libraries(scene_decoder_app audiointerfaces_static )
target_link_libraries(scene_decoder_app Boost::filesystem )
if( NOT Boost_USE_STATIC_LIBS )
  target_compile_definitions( scene_decoder_app PRIVATE BOOST_ALL_DYN_LINK )
endif( NOT Boost_USE_STATIC_LIBS )
target_compile_definitions( scene_decoder_app PRIVATE BOOST_ALL_NO_LIB )
