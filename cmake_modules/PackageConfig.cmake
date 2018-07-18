# Package generation configuration

SET( CPACK_DEBIAN_PACKAGE_MAINTAINER "Andreas Franck A.Franck@soton.ac.uk" )
SET( CPACK_PACKAGE_NAME "VISR" )
SET( CPACK_PACKAGE_VENDOR "ISVR")


# Use the version numbers centrally defined for the VISR project
set( CPACK_PACKAGE_VERSION_MAJOR ${VISR_MAJOR_VERSION} )
set( CPACK_PACKAGE_VERSION_MINOR ${VISR_MINOR_VERSION} )
set( CPACK_PACKAGE_VERSION_PATCH ${VISR_PATCH_VERSION} )


set( PKG_FILE_NAME ${VISR_TOPLEVEL_NAME}-${CMAKE_SYSTEM_NAME})
# Encode the Python major/minor version in the package file name if Python is enabled.
if( BUILD_PYTHON_BINDINGS )
  string( REGEX MATCH "[0-9]+.[0-9]+" PYTHON_MAJOR_MINOR ${PYTHONLIBS_VERSION_STRING} )
  set( PKG_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-python${PYTHON_MAJOR_MINOR}-${CMAKE_SYSTEM_NAME}" )
endif( BUILD_PYTHON_BINDINGS )

set( CPACK_PACKAGE_FILE_NAME ${PKG_FILE_NAME} )
#set( CPACK_SOURCE_PACKAGE_FILE_NAME ${CMAKE_INSTALL_PREFIX})
#set( CPACK_PACKAGE_INSTALL_DIRECTORY ${CMAKE_INSTALL_PREFIX})
SET( CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)



IF( WIN32 )
SET( CPACK_GENERATOR NSIS ZIP )
SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
GET_FILENAME_COMPONENT( PORTAUDIO_LIBRARY_DIR ${PORTAUDIO_LIBRARY} DIRECTORY )
INSTALL( FILES ${PORTAUDIO_LIBRARY_DIR}/portaudio_x64.dll DESTINATION 3rd)

GET_FILENAME_COMPONENT( SNDFILE_LIBRARY_DIR ${SNDFILE_LIBRARY} DIRECTORY )
INSTALL( FILES ${SNDFILE_LIBRARY_DIR}/libsndfile-1.dll DESTINATION 3rd)

# Boost
  GET_FILENAME_COMPONENT( BOOST_CHRONO_LIBRARY_BASENAME ${Boost_CHRONO_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_DATE_TIME_LIBRARY_BASENAME ${Boost_DATE_TIME_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_FILESYSTEM_LIBRARY_BASENAME ${Boost_FILESYSTEM_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_PROGRAM_OPTIONS_LIBRARY_BASENAME ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_SYSTEM_LIBRARY_BASENAME ${Boost_SYSTEM_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_THREAD_LIBRARY_BASENAME ${Boost_THREAD_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_REGEX_LIBRARY_BASENAME ${Boost_REGEX_LIBRARY_RELEASE} NAME_WE )
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_CHRONO_LIBRARY_BASENAME}.dll DESTINATION 3rd)
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_DATE_TIME_LIBRARY_BASENAME}.dll DESTINATION 3rd)
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_FILESYSTEM_LIBRARY_BASENAME}.dll DESTINATION 3rd)
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_PROGRAM_OPTIONS_LIBRARY_BASENAME}.dll DESTINATION 3rd)
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_SYSTEM_LIBRARY_BASENAME}.dll DESTINATION 3rd)
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_THREAD_LIBRARY_BASENAME}.dll DESTINATION 3rd)
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_CHRONO_LIBRARY_BASENAME}.dll DESTINATION 3rd)
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_REGEX_LIBRARY_BASENAME}.dll DESTINATION 3rd)
ENDIF( WIN32 )

IF( VISR_SYSTEM_NAME MATCHES "Linux" )
  SET( CPACK_GENERATOR DEB TBZ2 )
  SET( CPACK_DEBIAN_HOMEPAGE "http://www.s3a-spatialaudio.org" )
  SET( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
ENDIF(VISR_SYSTEM_NAME MATCHES "Linux")

IF( VISR_SYSTEM_NAME MATCHES "MacOS" )
  #SET( CPACK_GENERATOR PackageMaker )
  
  #disabled ZIP and TBZ2 
  SET( CPACK_GENERATOR DragNDrop )
  #SET( CPACK_GENERATOR Bundle )
  #SET( CPACK_PACKAGE_INSTALL_DIRECTORY "/Applications" )
  set( CPACK_PACKAGING_INSTALL_PREFIX "/${VISR_TOPLEVEL_NAME}")

  SET( CPACK_DMG_BACKGROUND_IMAGE ${CMAKE_SOURCE_DIR}/cmake_modules/resources/s3a_logo.jpg )
#  SET( CPACK_BUNDLE_NAME "VISR-0.9.0-Darwin/VISR" )
  SET( CPACK_BUNDLE_NAME "VISR" )
  SET( CPACK_BUNDLE_ICON ${CMAKE_SOURCE_DIR}/cmake_modules/resources/s3a_logo.png )
  SET( CPACK_BUNDLE_PLIST ${CMAKE_SOURCE_DIR}/cmake_modules/Info.plist )
  SET( CPACK_BUNDLE_STARTUP_COMMAND ${CMAKE_SOURCE_DIR}/cmake_modules/postscript.sh )  
  



#################################################################################################
# 3rd party library fixing

function(fix_rpath libpath)
	get_filename_component(libname ${${libpath}} NAME)

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
   
SET( FIX_LIBRARIES FLAC_LIBRARY OGG_LIBRARY VORBIS_LIBRARY VORBISENC_LIBRARY SNDFILE_LIBRARY PORTAUDIO_LIBRARIES BOOST_S BOOST_T BOOST_C BOOST_DT BOOST_FS BOOST_PO BOOST_R BOOST_TI BOOST_A)

################################################################################
# copying 3rd party libraries in "3rd" folder, and changing the ID to @rpath/<libraryname>.dylib. 

set (EXECUTE_COMMAND mkdir -p ${PROJECT_BINARY_DIR}/3rd)
execute_process(COMMAND ${EXECUTE_COMMAND} RESULT_VARIABLE rv)

foreach(v IN LISTS FIX_LIBRARIES)
    fix_rpath(${v})
endforeach()

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

######################################################################################################

    INSTALL( FILES ${PORTAUDIO_LIBRARIES} DESTINATION 3rd)
    INSTALL( FILES ${SNDFILE_LIBRARY} DESTINATION 3rd)  
    INSTALL( FILES ${FLAC_LIBRARY} DESTINATION 3rd )
    INSTALL( FILES ${OGG_LIBRARY} DESTINATION 3rd )
    INSTALL( FILES ${VORBIS_LIBRARY} DESTINATION 3rd )
    INSTALL( FILES ${VORBISENC_LIBRARY} DESTINATION 3rd )


   #SET(CPACK_PACKAGE_FILE_NAME VISR)

   # CPACK_PACKAGE_FILE_NAME - provides the name of the final compressed disk image (the name of the file that is distributed).
   # CPACK_PACKAGE_ICON - provides the icon for the mounted disk image (appears after the user mounts the disk image).
   

  If( NOT Boost_USE_STATIC_LIBS )   
    INSTALL( FILES ${BOOST_S} DESTINATION 3rd )
    INSTALL( FILES ${BOOST_T} DESTINATION 3rd )
    INSTALL( FILES ${BOOST_C} DESTINATION 3rd )
    INSTALL( FILES ${BOOST_DT} DESTINATION 3rd )
    INSTALL( FILES ${BOOST_FS} DESTINATION 3rd )
    INSTALL( FILES ${BOOST_PO} DESTINATION 3rd )
    INSTALL( FILES ${CMAKE_THREAD_LIBS_INIT} DESTINATION 3rd )
    INSTALL( FILES ${BOOST_R} DESTINATION 3rd )
    INSTALL( FILES ${BOOST_TI} DESTINATION 3rd )
    INSTALL( FILES ${BOOST_A} DESTINATION 3rd )
  ENDIF( NOT Boost_USE_STATIC_LIBS )

ENDIF( VISR_SYSTEM_NAME MATCHES "MacOS" )

INSTALL( DIRECTORY config DESTINATION ${VISR_TOPLEVEL_INSTALL_DIRECTORY} )
INSTALL( FILES ${CMAKE_SOURCE_DIR}/LICENSE.md DESTINATION ${VISR_TOPLEVEL_INSTALL_DIRECTORY} )
SET( CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.md )

# Install Python example scripts and templates
#INSTALL( DIRECTORY src/python/scripts DESTINATION python )
INSTALL( DIRECTORY src/python/templates DESTINATION ${PYTHON_MODULE_INSTALL_DIRECTORY} )
set( CPACK_DMG_VOLUME_NAME ${PKG_FILE_NAME})
#set(CPACK_POSTUPGRADE_SCRIPT "${CMAKE_SOURCE_DIR}/cmake_modules/pkg_rename.sh" ${PROJECT_BINARY_DIR}/${VISR_TOPLEVEL_NAME}.dmg ${PROJECT_BINARY_DIR}/PKG_FILE_NAME.dmg )
#set(CPACK_POSTUPGRADE_SCRIPT ${CMAKE_SOURCE_DIR}/cmake_modules/pkg_rename.sh)

INCLUDE( CPack )
