# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_DEFINITIONS( -DCMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}" )

ADD_EXECUTABLE( scene_decoder_app main.cpp signal_flow.cpp signal_flow.hpp )

TARGET_LINK_LIBRARIES(scene_decoder_app rcl)
TARGET_LINK_LIBRARIES(scene_decoder_app rrl)
TARGET_LINK_LIBRARIES(scene_decoder_app audiointerfaces)
