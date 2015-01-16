# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_DEFINITIONS( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

ADD_EXECUTABLE( scene_decoder_app main.cpp signal_flow.cpp signal_flow.hpp )


TARGET_LINK_LIBRARIES(scene_decoder_app pml)
TARGET_LINK_LIBRARIES(scene_decoder_app rbbl)
TARGET_LINK_LIBRARIES(scene_decoder_app rcl)
TARGET_LINK_LIBRARIES(scene_decoder_app objectmodel)
TARGET_LINK_LIBRARIES(scene_decoder_app panning)
TARGET_LINK_LIBRARIES(scene_decoder_app ril)
TARGET_LINK_LIBRARIES(scene_decoder_app rrl)
TARGET_LINK_LIBRARIES(scene_decoder_app efl)
TARGET_LINK_LIBRARIES(scene_decoder_app ${PORTAUDIO_LIBRARIES} )
TARGET_LINK_LIBRARIES(scene_decoder_app ${Boost_FILESYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(scene_decoder_app ${Boost_SYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(scene_decoder_app ${Boost_THREAD_LIBRARY} )
TARGET_LINK_LIBRARIES(scene_decoder_app efence )
