# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
initialisation_guard.cpp
python_wrapper.cpp
)

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS
initialisation_guard.hpp
python_wrapper.hpp
)

INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIR} )

ADD_LIBRARY( pythonsupport STATIC ${SOURCES} ${HEADERS} )
# Transitive dependencies
target_link_libraries( pythonsupport visr_static )
target_link_libraries( pythonsupport ${PYTHON_LIBRARY} )
target_link_libraries( pythonsupport ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( pythonsupport ${Boost_SYSTEM_LIBRARY} )

# TODO: DO we need a shared library and/or a position-independent static version?

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
