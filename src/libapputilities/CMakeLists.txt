# Copyright Institute of Sound and Vibration Research - All rights reserved

set( SOURCES options.cpp )
set( HEADERS export_symbols.hpp options.hpp )

add_library( apputilities_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( apputilities_static PROPERTIES OUTPUT_NAME apputilities )
target_link_libraries( apputilities_static ${Boost_PROGRAM_OPTIONS_LIBRARY})

add_library( apputilities_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( apputilities_shared PROPERTIES OUTPUT_NAME apputilities )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( apputilities_shared PROPERTIES ARCHIVE_OUTPUT_NAME apputilities_shared )
target_compile_definitions( apputilities_shared PRIVATE -DVISR_BUILD_APPUTILITIES_SHARED_LIBRARY=1 -DVISR_BUILD_APPUTILITIES_LIBRARY=1)
target_compile_definitions( apputilities_shared PUBLIC -DVISR_BUILD_APPUTILITIES_SHARED_LIBRARY=1)
set_target_properties( apputilities_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
target_link_libraries( apputilities_shared ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries( apputilities_shared ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries( apputilities_shared ${Boost_SYSTEM_LIBRARY})

install(TARGETS apputilities_static ARCHIVE DESTINATION lib )
install(TARGETS apputilities_shared ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib )
