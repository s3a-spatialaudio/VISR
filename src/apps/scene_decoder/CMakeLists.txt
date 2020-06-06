# Copyright Institute of Sound and Vibration Research - All rights reserved

add_definitions( -DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" )

add_executable( scene_decoder_app main.cpp signal_flow.cpp signal_flow.hpp )

target_link_libraries(scene_decoder_app PRIVATE rcl_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(scene_decoder_app PRIVATE rrl_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(scene_decoder_app PRIVATE audiointerfaces_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(scene_decoder_app PRIVATE Boost::filesystem )
if( NOT Boost_USE_STATIC_LIBS )
  target_compile_definitions( scene_decoder_app PRIVATE -DBOOST_ALL_DYN_LINK )
endif( NOT Boost_USE_STATIC_LIBS )
target_compile_definitions( scene_decoder_app PRIVATE -DBOOST_ALL_NO_LIB )
