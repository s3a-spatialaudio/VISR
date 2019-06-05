# Copyright Institute of Sound and Vibration Research - All rights reserved

add_executable( feedthrough_app
  main.cpp
  options.hpp
  options.cpp
  signal_flow.cpp )

target_link_libraries(feedthrough_app PRIVATE rcl_shared  )
target_link_libraries(feedthrough_app PRIVATE visr_shared )
target_link_libraries(feedthrough_app PRIVATE rrl_shared )
target_link_libraries(feedthrough_app PRIVATE apputilities_shared )
target_link_libraries(feedthrough_app PRIVATE audiointerfaces_shared )
target_link_libraries(feedthrough_app PRIVATE Boost::filesystem )
