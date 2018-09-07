# Copyright Institute of Sound and Vibration Research - All rights reserved

set( SOURCES options.cpp )
set( HEADERS export_symbols.hpp options.hpp )

add_library( apputilities_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( apputilities_static PROPERTIES OUTPUT_NAME apputilities )
target_link_libraries( apputilities_static PUBLIC Boost::program_options )
target_link_libraries( apputilities_static PRIVATE Boost::filesystem )

add_library( apputilities_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( apputilities_shared PROPERTIES OUTPUT_NAME apputilities )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( apputilities_shared PROPERTIES ARCHIVE_OUTPUT_NAME apputilities_shared )
target_compile_definitions( apputilities_shared PRIVATE -DVISR_BUILD_APPUTILITIES_SHARED_LIBRARY=1 -DVISR_BUILD_APPUTILITIES_LIBRARY=1)
target_compile_definitions( apputilities_shared PUBLIC -DVISR_BUILD_APPUTILITIES_SHARED_LIBRARY=1)
set_target_properties( apputilities_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
target_link_libraries( apputilities_shared PUBLIC Boost::program_options )
target_link_libraries( apputilities_shared PRIVATE Boost::filesystem )

# Set public headers to be installed for the shared target.
# Because the headers are the same for the static library, the headers are only
# installled once.
set_target_properties(apputilities_shared PROPERTIES PUBLIC_HEADER "${HEADERS}" )

# Set include paths for dependent projects
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
foreach(TARGET_TYPE static shared)
  target_include_directories(apputilities_${TARGET_TYPE}
    PUBLIC
    $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/visr>
  )
endforeach()

install(TARGETS apputilities_static EXPORT VISR_exports ARCHIVE DESTINATION lib )
install(TARGETS apputilities_shared EXPORT VISR_exports ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/visr/libapputilities )
