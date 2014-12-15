# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_EXECUTABLE( gain_matrix_app main.cpp signal_flow.cpp )

TARGET_LINK_LIBRARIES(gain_matrix_app rbbl)
TARGET_LINK_LIBRARIES(gain_matrix_app rcl)
TARGET_LINK_LIBRARIES(gain_matrix_app ril)
TARGET_LINK_LIBRARIES(gain_matrix_app rrl)
TARGET_LINK_LIBRARIES(gain_matrix_app efl)
TARGET_LINK_LIBRARIES(gain_matrix_app portaudio)
