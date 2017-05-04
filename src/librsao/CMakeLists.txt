# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
late_reverb_filter_calculator.cpp
late_reverb_parameter.cpp
reverb_object_renderer.cpp
reverb_parameter_calculator.cpp
)

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS
late_reverb_filter_calculator.hpp
late_reverb_parameter.hpp
reverb_object_renderer.hpp
reverb_parameter_calculator.hpp
)

# Preliminary hack. Used only for scene_sender. 
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/3rd/include/oscpkt )


ADD_LIBRARY( rsao STATIC ${SOURCES} ${HEADERS} )

# Transitive dependencies
target_link_libraries( rsao pml )
target_link_libraries( rsao rcl )
target_link_libraries( rsao ril )
target_link_libraries( rsao objectmodel )
target_link_libraries( rsao efl )

target_link_libraries( rsao ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( rsao ${Boost_SYSTEM_LIBRARY} )

# Add the unit test directory.
# ADD_SUBDIRECTORY( test )
