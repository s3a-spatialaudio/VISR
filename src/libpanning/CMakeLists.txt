# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
 AllRAD.cpp
 CAP.cpp
 LoudspeakerArray.cpp
 VBAP.cpp
 XYZ.cpp
)

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS
 defs.h
 export_symbols.hpp
 AllRAD.h
 CAP.h
 LoudspeakerArray.h
 VBAP.h
 XYZ.h
)

# Workaround for GCC <= 4.8:
# Because the regex implementation for this 's standard library is completely broken, we switch to GCC.
# Otherwise C++11's regex is preferred because it avoids the external shared library dependency.
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9 )
  set_source_files_properties( LoudspeakerArray.cpp PROPERTIES COMPILE_DEFINITIONS VISR_USE_BOOST_REGEX=1 )
endif (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9 )

add_library( panning_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( panning_static PROPERTIES OUTPUT_NAME panning )

add_library( panning_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( panning_shared PROPERTIES OUTPUT_NAME panning )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( panning_shared PROPERTIES ARCHIVE_OUTPUT_NAME panning_shared )
target_compile_definitions( panning_shared PRIVATE -DVISR_BUILD_PANNING_SHARED_LIBRARY=1 -DVISR_BUILD_PANNING_LIBRARY=1)
target_compile_definitions( panning_shared PUBLIC -DVISR_BUILD_PANNING_SHARED_LIBRARY=1)
set_target_properties( panning_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )

# Set target properties common to both library types.
foreach(TARGET_TYPE static shared)
  target_link_libraries( panning_${TARGET_TYPE} PUBLIC rbbl_${TARGET_TYPE} )
  if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9 )
    target_link_libraries( panning_${TARGET_TYPE} PRIVATE Boost::regex )
  endif (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9 )
  target_link_libraries( panning_${TARGET_TYPE} PRIVATE Boost::filesystem )
  if( NOT Boost_USE_STATIC_LIBS )
    target_compile_definitions( panning_${TARGET_TYPE} PRIVATE -DBOOST_ALL_DYN_LINK )
  endif( NOT Boost_USE_STATIC_LIBS )
  target_compile_definitions( panning_${TARGET_TYPE} PRIVATE -DBOOST_ALL_NO_LIB )
endforeach()

# Set public headers to be installed for the shared target.
# Because the headers are the same for the static library, the headers are only
# installled once.
set_target_properties( panning_shared PROPERTIES PUBLIC_HEADER "${HEADERS}" )


# Set include paths for dependent projects
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
foreach(TARGET_TYPE static shared)
  target_include_directories(panning_${TARGET_TYPE}
    PUBLIC
    $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/visr>
  )
endforeach()

install(TARGETS panning_static
        EXPORT VISR_exports
        ARCHIVE DESTINATION lib COMPONENT static_libraries )
install(TARGETS panning_shared
        EXPORT VISR_exports
        ARCHIVE DESTINATION lib COMPONENT development_files 
        RUNTIME DESTINATION lib COMPONENT shared_libraries
        LIBRARY DESTINATION lib COMPONENT shared_libraries
        PUBLIC_HEADER DESTINATION include/visr/libpanning COMPONENT development_files )

# Add the unit tests.
ADD_SUBDIRECTORY( test )
