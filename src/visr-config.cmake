
# Export the Boost dependency for imported targets.
set(BOOST_ROOT @BOOST_ROOT@ CACHE PATH "In case boost was relocated.")
set(Boost_USE_MULTITHREADED @Boost_USE_MULTITHREADED@)
set(Boost_USE_STATIC LIBS @Boost_USE_STATIC_LIBS@)
find_package(Boost @VISR_BOOST_MINIMUM_VERSION@ REQUIRED COMPONENTS @VISR_BOOST_LIBRARIES@ )

# TODO Resolve other third party dependencies if required.

# import targets
include(${CMAKE_CURRENT_LIST_DIR}/visr_exports.cmake)
