# Copyright Institute of Sound and Vibration Research - All rights reserved

# 3rd party library fixing required on Mac OS X

function(fix_rpath libpath )
  get_filename_component( libname ${${libpath}} NAME )
	set (EXECUTE_COMMAND  cp ${${libpath}} ${PROJECT_BINARY_DIR}/3rd/${libname})
	execute_process(COMMAND ${EXECUTE_COMMAND} RESULT_VARIABLE rv)

	SET( ${libpath} ${PROJECT_BINARY_DIR}/3rd/${libname})
	SET( ${libpath} ${PROJECT_BINARY_DIR}/3rd/${libname} PARENT_SCOPE ) 

	set (EXECUTE_COMMAND  chmod 777 ${PROJECT_BINARY_DIR}/3rd/${libname})
	execute_process(COMMAND ${EXECUTE_COMMAND} RESULT_VARIABLE rv)

	set (EXECUTE_COMMAND install_name_tool -id @rpath/${libname} ${${libpath}})
	message( STATUS " output " ${libname} ": " ${${libpath}} )
	execute_process(COMMAND ${EXECUTE_COMMAND} RESULT_VARIABLE rv)
endfunction()

# function(fix_rpath libpath)
#   fix_rpath_and_name( libpath ${libname} )
# endfunction()

function(fix_dependencies_of_3rdparty depname libpath)

	set (EXECUTE_COMMAND bash "-c" "${CMAKE_SOURCE_DIR}/cmake_modules/./change_dependency_installname.sh ${depname} ${libpath}" )

	execute_process(COMMAND ${EXECUTE_COMMAND} OUTPUT_VARIABLE rv)
	#message( STATUS "output "${depname} ": " ${rv} )
endfunction()

    get_target_property(BOOST_S Boost::system IMPORTED_LOCATION)
    get_target_property(BOOST_T Boost::thread IMPORTED_LOCATION)
    get_target_property(BOOST_C Boost::chrono IMPORTED_LOCATION)
    get_target_property(BOOST_DT Boost::date_time IMPORTED_LOCATION)
    get_target_property(BOOST_FS Boost::filesystem IMPORTED_LOCATION)
    get_target_property(BOOST_PO Boost::program_options IMPORTED_LOCATION)
    get_target_property(BOOST_R Boost::regex IMPORTED_LOCATION)
    get_target_property(BOOST_TI Boost::timer IMPORTED_LOCATION)
    get_target_property(BOOST_A Boost::atomic IMPORTED_LOCATION)

    get_filename_component(BOOST_S ${BOOST_S} REALPATH)
    get_filename_component(BOOST_T ${BOOST_T} REALPATH)
    get_filename_component(BOOST_C ${BOOST_C} REALPATH)
    get_filename_component(BOOST_DT ${BOOST_DT} REALPATH)
    get_filename_component(BOOST_FS ${BOOST_FS} REALPATH)
    get_filename_component(BOOST_PO ${BOOST_PO} REALPATH)
    get_filename_component(BOOST_R ${BOOST_R} REALPATH) 
    get_filename_component(BOOST_TI ${BOOST_TI} REALPATH)    
    get_filename_component(BOOST_A ${BOOST_A} REALPATH)
   
SET( FIX_LIBRARIES FLAC_LIBRARY OGG_LIBRARY VORBIS_LIBRARY VORBISENC_LIBRARY SNDFILE_LIBRARY PORTAUDIO_LIBRARIES BOOST_S BOOST_T BOOST_C BOOST_DT BOOST_FS BOOST_PO BOOST_R BOOST_TI BOOST_A PYTHON_LIBRARY )

################################################################################
# copying 3rd party libraries in "3rd" folder, and changing the ID to @rpath/<libraryname>.dylib. 

set (EXECUTE_COMMAND mkdir -p ${PROJECT_BINARY_DIR}/3rd)
execute_process(COMMAND ${EXECUTE_COMMAND} RESULT_VARIABLE rv)

foreach(v IN LISTS FIX_LIBRARIES)
    fix_rpath(${v})
endforeach()

message( STATUS "Python library after adjust: " ${PYTHON_LIBRARY} )

################################################################################
# Updating the imported library location
  set_target_properties(Boost::system PROPERTIES IMPORTED_LOCATION ${BOOST_S} IMPORTED_LOCATION_DEBUG ${BOOST_S} IMPORTED_LOCATION_RELEASE ${BOOST_S}) 
  set_target_properties(Boost::thread PROPERTIES IMPORTED_LOCATION ${BOOST_T} IMPORTED_LOCATION_DEBUG ${BOOST_T} IMPORTED_LOCATION_RELEASE ${BOOST_T}) 
  set_target_properties(Boost::chrono PROPERTIES IMPORTED_LOCATION ${BOOST_C} IMPORTED_LOCATION_DEBUG ${BOOST_C} IMPORTED_LOCATION_RELEASE ${BOOST_C}) 
  set_target_properties(Boost::date_time PROPERTIES IMPORTED_LOCATION ${BOOST_DT} IMPORTED_LOCATION_DEBUG ${BOOST_DT} IMPORTED_LOCATION_RELEASE ${BOOST_DT}) 
  set_target_properties(Boost::filesystem PROPERTIES IMPORTED_LOCATION ${BOOST_FS} IMPORTED_LOCATION_DEBUG ${BOOST_FS} IMPORTED_LOCATION_RELEASE ${BOOST_FS}) 
  set_target_properties(Boost::program_options PROPERTIES IMPORTED_LOCATION ${BOOST_PO} IMPORTED_LOCATION_DEBUG ${BOOST_PO} IMPORTED_LOCATION_RELEASE ${BOOST_PO}) 
  set_target_properties(Boost::regex PROPERTIES IMPORTED_LOCATION ${BOOST_R} IMPORTED_LOCATION_DEBUG ${BOOST_R} IMPORTED_LOCATION_RELEASE ${BOOST_R}) 
  set_target_properties(Boost::timer PROPERTIES IMPORTED_LOCATION ${BOOST_TI} IMPORTED_LOCATION_DEBUG ${BOOST_TI} IMPORTED_LOCATION_RELEASE ${BOOST_TI})
  set_target_properties(Boost::atomic PROPERTIES IMPORTED_LOCATION ${BOOST_A} IMPORTED_LOCATION_DEBUG ${BOOST_A} IMPORTED_LOCATION_RELEASE ${BOOST_A}) 

################################################################################
# Fixing internal dependencies of third party libraries. 
# This is no longer required for boost libraries when using the imported target "Boost::<libraryname>" syntax

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
