# Copyright Institute of Sound and Vibration Research - All rights reserved

set( SOURCES options.cpp )
set( HEADERS options.hpp )

add_library( apputilities STATIC ${SOURCES} ${HEADERS} )

target_link_libraries( apputilities ${Boost_PROGRAM_OPTIONS_LIBRARY})