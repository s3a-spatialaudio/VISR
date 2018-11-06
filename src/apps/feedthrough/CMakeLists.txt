# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_EXECUTABLE( feedthrough_app main.cpp signal_flow.cpp )

TARGET_LINK_LIBRARIES(feedthrough_app rcl_shared  )
TARGET_LINK_LIBRARIES(feedthrough_app visr_shared )
TARGET_LINK_LIBRARIES(feedthrough_app rrl_shared )
TARGET_LINK_LIBRARIES(feedthrough_app audiointerfaces_shared )
set_target_properties(feedthrough_app PROPERTIES SKIP_BUILD_RPATH FALSE)