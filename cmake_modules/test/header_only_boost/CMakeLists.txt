# Copyright Institute of Sound and Vibration Research - All rights reserved

# Configure-time test: Adjust3rdPartyLibraries.cmake must cope with a header-only
# Boost library (modern Boost.Regex without ICU). Such a library is an INTERFACE
# imported target without IMPORTED_LOCATION, so there is nothing to copy or relink.
# Before the fix, fixBoostLibrary() ran cp, chmod and install_name_tool on a file
# named LIBLOCATION-NOTFOUND and set it as the target's IMPORTED_LOCATION, which
# the install rules then tried to install.

cmake_minimum_required( VERSION 3.12 )
project( header_only_boost_test NONE )

set( VISR_SYSTEM_NAME MacOS )
set( VISR_BUILD_3RD_PARTY_RUNTIME_LIBRARY_DIR ${PROJECT_BINARY_DIR}/3rd )
set( VISR_BOOST_LIBRARIES regex )

add_library( Boost::regex INTERFACE IMPORTED )

include( ${CMAKE_CURRENT_SOURCE_DIR}/../../Adjust3rdPartyLibraries.cmake )

get_target_property( REGEX_LOCATION Boost::regex IMPORTED_LOCATION )
# Compare as strings: if( <variable> ) treats any value ending in -NOTFOUND as false.
if( NOT "${REGEX_LOCATION}" STREQUAL "" AND NOT "${REGEX_LOCATION}" STREQUAL "REGEX_LOCATION-NOTFOUND" )
  message( FATAL_ERROR "Header-only Boost::regex was given an IMPORTED_LOCATION: ${REGEX_LOCATION}" )
endif()
