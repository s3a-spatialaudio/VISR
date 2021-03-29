@PACKAGE_INIT@

set( VISR_STATIC_LIBRARIES @BUILD_INSTALL_STATIC_LIBRARIES@ )
set( VISR_STATIC_PIC_LIBRARIES @BUILD_INSTALL_STATIC_PIC_LIBRARIES@ )
set( VISR_SHARED_LIBRARIES @BUILD_INSTALL_SHARED_LIBRARIES@ )
# List of the installed VISR library (w/o suffixes)
set( VISR_LIBRARIES @VISR_LIBRARIES@ )

set( VISR_USE_STATIC_BOOST_LIBRARIES @Boost_USE_STATIC_LIBS@ )
set( VISR_USE_SNDFILE_LIBRARY @BUILD_USE_SNDFILE_LIBRARY@ )
set( VISR_THREAD_SUPPORT_DISABLED @BUILD_DISABLE_THREADS@ )
set( VISR_AUDIOINTERFACES_PORTAUDIO @BUILD_AUDIOINTERFACES_PORTAUDIO@ )
set( VISR_AUDIOINTERFACES_JACK @BUILD_AUDIOINTERFACES_JACK@ )

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

  # Note: we do not list the dependencies SndFile, Portaudio, Jack 
  # here, because they are non-standard and cannot imported through 
  # an export config. I.e., it would require a FindXXX.cmake file in
  # every dependent project.
endif( ${VISR_STATIC_LIBRARIES} OR ${VISR_STATIC_PIC_LIBRARIES} )

# import targets
include(${CMAKE_CURRENT_LIST_DIR}/visr_exports.cmake)

# Check whether the requirements of the exported targets are met.
include(CMakePackageConfigHelpers)
foreach( lib ${VISR_LIBRARIES} )
  if( ${VISR_SHARED_LIBRARIES} )
    check_required_components( ${lib}_shared )
  endif( ${VISR_SHARED_LIBRARIES} )
  if( ${VISR_STATIC_LIBRARIES} )
    check_required_components( ${lib}_static )
  endif( ${VISR_STATIC_LIBRARIES} )
  if( ${VISR_STATIC_PIC_LIBRARIES} )
    check_required_components( ${lib}_static_pic )
  endif( ${VISR_STATIC_PIC_LIBRARIES} )
endforeach( lib ${VISR_LIBRARIES} )