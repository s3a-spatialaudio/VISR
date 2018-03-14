# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
gil_ensure_guard.cpp
initialisation_guard.cpp
load_module.cpp
)

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS
export_symbols.hpp
gil_ensure_guard.hpp
initialisation_guard.hpp
load_module.hpp
)

INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIR} )

ADD_LIBRARY( pythonsupport_static STATIC ${SOURCES} ${HEADERS} )
# Transitive dependencies
target_link_libraries( pythonsupport_static ${PYTHON_LIBRARY} )
target_link_libraries( pythonsupport_static ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( pythonsupport_static ${Boost_SYSTEM_LIBRARY} )

add_library( pythonsupport_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( pythonsupport_shared PROPERTIES OUTPUT_NAME pythonsupport )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( pythonsupport_shared PROPERTIES ARCHIVE_OUTPUT_NAME pythonsupport_shared )
target_compile_definitions( pythonsupport_shared PRIVATE -DVISR_BUILD_PYTHONSUPPORT_SHARED_LIBRARY=1 -DVISR_BUILD_PYTHONSUPPORT_LIBRARY=1)
target_compile_definitions( pythonsupport_shared PUBLIC -DVISR_BUILD_PYTHONSUPPORT_SHARED_LIBRARY=1)
set_target_properties( pythonsupport_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
target_link_libraries( pythonsupport_shared ${PYTHON_LIBRARY} )
target_link_libraries( pythonsupport_shared ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( pythonsupport_shared ${Boost_SYSTEM_LIBRARY} )

install(TARGETS pythonsupport_static ARCHIVE DESTINATION lib )
install(TARGETS pythonsupport_shared ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib )

# Add the unit test directory.
# No unit test for this basic functionality yet.
# see libpythoncomponents/test
# ADD_SUBDIRECTORY( test )
