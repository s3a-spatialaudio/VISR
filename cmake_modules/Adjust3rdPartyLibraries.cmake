# Copyright Institute of Sound and Vibration Research - All rights reserved

# 3rd party library fixing required on Mac OS X

function(fix_rpath libpath )
  get_filename_component( libname ${${libpath}} NAME )
  set (EXECUTE_COMMAND  cp ${${libpath}} ${PROJECT_BINARY_DIR}/3rd/${libname})
  execute_process(COMMAND ${EXECUTE_COMMAND} RESULT_VARIABLE rv)

  set( ${libpath} ${PROJECT_BINARY_DIR}/3rd/${libname})
  set( ${libpath} ${PROJECT_BINARY_DIR}/3rd/${libname} PARENT_SCOPE )

  set (EXECUTE_COMMAND  chmod 777 ${PROJECT_BINARY_DIR}/3rd/${libname})
  execute_process(COMMAND ${EXECUTE_COMMAND} RESULT_VARIABLE rv)

  set (EXECUTE_COMMAND install_name_tool -id @rpath/${libname} ${${libpath}})
  #  message( STATUS " output " ${libname} ": " ${${libpath}} )
  execute_process(COMMAND ${EXECUTE_COMMAND} RESULT_VARIABLE rv)
endfunction()

################################################################################
# copying 3rd party libraries in "3rd" folder, and changing the ID to @rpath/<libraryname>.dylib. 

set( FIX_LIBRARIES FLAC_LIBRARY OGG_LIBRARY VORBIS_LIBRARY VORBISENC_LIBRARY SNDFILE_LIBRARY PORTAUDIO_LIBRARIES )

if( BUILD_PYTHON_BINDINGS )
 list( APPEND FIX_LIBRARIES PYTHON_LIBRARY )
endif( BUILD_PYTHON_BINDINGS )

file( MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/3rd )
 
foreach(v IN LISTS FIX_LIBRARIES)
    fix_rpath(${v})
endforeach()

################################################################################
# Do the same for the boost libs.

function( fixBoostLibrary BOOSTLIB )
  get_target_property( LIBLOCATION Boost::${BOOSTLIB} IMPORTED_LOCATION)
  get_filename_component( LIBREALPATH ${LIBLOCATION} REALPATH)
  fix_rpath( LIBREALPATH )
  get_filename_component( LIBNAME ${LIBREALPATH} NAME )
  set( ADJUSTEDLIBPATH ${PROJECT_BINARY_DIR}/3rd/${LIBNAME})
  set_target_properties( Boost::${BOOSTLIB} PROPERTIES
                         IMPORTED_LOCATION ${ADJUSTEDLIBPATH}
			 IMPORTED_LOCATION_DEBUG ${ADJUSTEDLIBPATH}
			 IMPORTED_LOCATION_RELEASE ${ADJUSTEDLIBPATH})
endfunction()

foreach( BOOSTLIB ${VISR_BOOST_LIBRARIES} )
  fixBoostLibrary( ${BOOSTLIB} )
endforeach()

################################################################################
# Fixing internal dependencies of third party libraries. 
# This is no longer required for boost libraries when using the imported target "Boost::<libraryname>" syntax

function(fix_dependencies_of_3rdparty depname libpath)
  set (EXECUTE_COMMAND bash "-c" "${CMAKE_SOURCE_DIR}/cmake_modules/./change_dependency_installname.sh ${depname} ${libpath}" )
  execute_process(COMMAND ${EXECUTE_COMMAND} OUTPUT_VARIABLE rv)
endfunction()

get_filename_component(FLAC_LIBRARY_NAME ${FLAC_LIBRARY} NAME_WE)
get_filename_component(OGG_LIBRARY_NAME ${OGG_LIBRARY} NAME_WE)
get_filename_component(VORBIS_LIBRARY_NAME ${VORBIS_LIBRARY} NAME_WE)
get_filename_component(VORBISENC_LIBRARY_NAME ${VORBISENC_LIBRARY} NAME_WE)

fix_dependencies_of_3rdparty(${OGG_LIBRARY_NAME} ${FLAC_LIBRARY})
fix_dependencies_of_3rdparty(${FLAC_LIBRARY_NAME} ${SNDFILE_LIBRARY})
fix_dependencies_of_3rdparty(${OGG_LIBRARY_NAME} ${SNDFILE_LIBRARY})
fix_dependencies_of_3rdparty(${VORBIS_LIBRARY_NAME} ${SNDFILE_LIBRARY})
fix_dependencies_of_3rdparty(${VORBISENC_LIBRARY_NAME} ${SNDFILE_LIBRARY})
fix_dependencies_of_3rdparty(${OGG_LIBRARY_NAME} ${VORBIS_LIBRARY})
fix_dependencies_of_3rdparty(${OGG_LIBRARY_NAME} ${VORBISENC_LIBRARY})
fix_dependencies_of_3rdparty(${VORBIS_LIBRARY_NAME} ${VORBISENC_LIBRARY})
