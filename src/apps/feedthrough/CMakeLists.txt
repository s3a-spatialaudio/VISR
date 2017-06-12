# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_EXECUTABLE( feedthrough_app main.cpp signal_flow.cpp )

TARGET_LINK_LIBRARIES(feedthrough_app audiointerfaces)
TARGET_LINK_LIBRARIES(feedthrough_app rcl)
#TARGET_LINK_LIBRARIES(feedthrough_app rrl)
TARGET_LINK_LIBRARIES(feedthrough_app visr_impl)
TARGET_LINK_LIBRARIES(feedthrough_app objectmodel)
TARGET_LINK_LIBRARIES(feedthrough_app pml)
TARGET_LINK_LIBRARIES(feedthrough_app efl)
TARGET_LINK_LIBRARIES(feedthrough_app ${Boost_FILESYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(feedthrough_app ${Boost_SYSTEM_LIBRARY} )

