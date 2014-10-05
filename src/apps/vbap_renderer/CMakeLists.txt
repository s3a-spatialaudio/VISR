# Copyright Institute of Sound and Vibratioin Research - All rights reserved

ADD_EXECUTABLE( vbap_renderer main.cpp )
TARGET_LINK_LIBRARIES(vbap_renderer efl)
TARGET_LINK_LIBRARIES(vbap_renderer ril)
TARGET_LINK_LIBRARIES(vbap_renderer rrl)
