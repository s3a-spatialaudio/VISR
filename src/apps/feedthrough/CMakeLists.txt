# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_EXECUTABLE( feedthrough_app main.cpp signal_flow.cpp )

TARGET_LINK_LIBRARIES(feedthrough_app rcl)
TARGET_LINK_LIBRARIES(feedthrough_app ril)
TARGET_LINK_LIBRARIES(feedthrough_app rrl)
TARGET_LINK_LIBRARIES(feedthrough_app efl)
TARGET_LINK_LIBRARIES(feedthrough_app ${PORTAUDIO_LIBRARIES} )
