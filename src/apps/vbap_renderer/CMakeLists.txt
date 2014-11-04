# Copyright Institute of Sound and Vibratioin Research - All rights reserved

ADD_EXECUTABLE( vbap_renderer main.cpp signal_flow.cpp )

TARGET_LINK_LIBRARIES(vbap_renderer rcl)
TARGET_LINK_LIBRARIES(vbap_renderer ril)
TARGET_LINK_LIBRARIES(vbap_renderer rrl)
TARGET_LINK_LIBRARIES(vbap_renderer efl)
TARGET_LINK_LIBRARIES(vbap_renderer portaudio)
