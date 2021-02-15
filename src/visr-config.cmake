@PACKAGE_INIT@

set( VISR_STATIC_LIBRARIES @BUILD_INSTALL_STATIC_LIBRARIES@ )
set( VISR_STATIC_PIC_LIBRARIES @BUILD_INSTALL_STATIC_PIC_LIBRARIES@ )
set( VISR_SHARED_LIBRARIES @BUILD_INSTALL_SHARED_LIBRARIES@ )

set( VISR_USE_STATIC_BOOST_LIBRARIES @Boost_USE_STATIC_LIBS@ )
set( VISR_THREAD_SUPPORT_DISABLED @BUILD_DISABLE_THREADS@ )

if( ${VISR_STATIC_LIBRARIES} OR ${VISR_STATIC_PIC_LIBRARIES} )
  # Export dependencies for exported static targets (incl. static_pic)
  # if any of these variants is built.
  include(CMakeFindDependencyMacro)

  set( Boost_USE_MULTITHREADED @Boost_USE_MULTITHREADED@ )
  set( Boost_USE_STATIC_LIBS @Boost_USE_STATIC_LIBS@ )

  find_dependency(Boost @Boost_VERSION_STRING@ EXACT
    COMPONENTS @VISR_BOOST_LIBRARIES@ )
  if( NOT ${VISR_THREAD_SUPPORT_DISABLED} )
    find_dependency( Threads )
  endif( NOT ${VISR_THREAD_SUPPORT_DISABLED} )

  # TODO: Soundfile, portaudio, Jack
endif( ${VISR_STATIC_LIBRARIES} OR ${VISR_STATIC_PIC_LIBRARIES} )

# import targets
include(${CMAKE_CURRENT_LIST_DIR}/visr_exports.cmake)
