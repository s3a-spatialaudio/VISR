# Copyright Institute of Sound and Vibration Research - All rights reserved

# Top-level build configuration for project VISceRal

# Arbitrarily set minimum version.
# This specific version is used to avoid complaints about the 
# cygwin platform not defining WIN32 anymore.
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.4)

# Avoid warnings about the CXX_VISIBILITY_PRESET set for all target types.
if( POLICY CMP0063 ) # (only if the CMake version supports this policy
  CMAKE_POLICY(SET CMP0063 NEW)
endif( POLICY CMP0063 )

# The name of our top-level project.
PROJECT( VISR CXX C )

# Define central version numbers for the project
# TODO: Decide whether to move that to an external CMake file .
set( VISR_MAJOR_VERSION 0 )
set( VISR_MINOR_VERSION 9 )
set( VISR_PATCH_VERSION 0 )

ADD_DEFINITIONS( -DVISR_MAJOR_VERSION="${VISR_MAJOR_VERSION}" )
ADD_DEFINITIONS( -DVISR_MINOR_VERSION="${VISR_MINOR_VERSION}" )
ADD_DEFINITIONS( -DVISR_PATCH_VERSION="${VISR_PATCH_VERSION}" )

# Add support for unit tests
INCLUDE( CTest )

# Enable C++11 features on Linux platforms
# See http://www.cmake.org/Wiki/CMake/Tutorials/C%2B%2B11Flags
# Maybe there is a more general way to configure the different platforms and toolchains
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")
#message( STATUS "output "${CMAKE_MODULE_PATH} )
# At the moment, all include directories are relative to the source root directory.
# Therefore it suffices to set this root as the include path.
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src )

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake_modules)

# Default path for static libraries
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/staticlib )

# Copy the executabl  es to the binary directory
SET( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )

# Default destination for shared libraries (not DLLs)
SET( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib )

# Default path for DLLs on windows
SET( RUNTIME_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib )

#############################################################
# Create a system string.
SET( VISR_SYSTEM_NAME ${CMAKE_SYSTEM_NAME} )
IF( VISR_SYSTEM_NAME MATCHES "Darwin" )

SET( VISR_TOPLEVEL_NAME VISR-${VISR_MAJOR_VERSION}.${VISR_MINOR_VERSION}.${VISR_PATCH_VERSION})
SET(CMAKE_INSTALL_PREFIX /Applications/${VISR_TOPLEVEL_NAME})

SET( CMAKE_MACOSX_RPATH ON)
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already
# (but later on when installing)

# <af> Temporarily disabled this feature to enable building without installing.
# <af> This probably breaks the package building mechanism.
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/3rd;${CMAKE_INSTALL_PREFIX}/lib")
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
#SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


   SET( VISR_SYSTEM_NAME MacOS )
#  SET(CMAKE_INSTALL_PREFIX /Applications )
ENDIF( VISR_SYSTEM_NAME MATCHES "Darwin" )

SET( VISR_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR} )
IF( VISR_SYSTEM_PROCESSOR MATCHES "AMD64" )
  SET( VISR_SYSTEM_PROCESSOR x86_64 )
ENDIF( VISR_SYSTEM_PROCESSOR MATCHES "AMD64" )
STRING( TOLOWER ${VISR_SYSTEM_NAME}-${VISR_SYSTEM_PROCESSOR} VISR_PLATFORM_NAME )
# For use as a C/C++ preprocessor definition, we must not use hyphens.
STRING( TOLOWER ${VISR_SYSTEM_NAME}_${VISR_SYSTEM_PROCESSOR} VISR_PLATFORM_NAME_MACRO )


# MESSAGE( STATUS "VISR system processor: " ${VISR_SYSTEM_PROCESSOR} )
# MESSAGE( STATUS "VISR system name:      " ${VISR_SYSTEM_NAME} )
# MESSAGE( STATUS "VISR platform name:    " ${VISR_PLATFORM_NAME} )

# Make the platform strings available to the source code.
ADD_DEFINITIONS("-DVISR_SYSTEM_PROCESSOR_${VISR_SYSTEM_PROCESSOR}")
ADD_DEFINITIONS("-DVISR_SYSTEM_NAME_${VISR_SYSTEM_NAME}")
ADD_DEFINITIONS("-DVISR_PLATFORM_NAME_${VISR_PLATFORM_NAME_MACRO}")

############################################################
# Set platform-specific compiler flags

# This covers Linux and MacOS X
if(UNIX)
  # Support for the Raspberry Pi: Enable hardware floating-point arithmetic
  if( ${VISR_SYSTEM_PROCESSOR} MATCHES "armv7l" )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon -march=armv7-a -mfloat-abi=hard" )
  endif( ${VISR_SYSTEM_PROCESSOR} MATCHES "armv7l" )
  # TODO: Add more elaborate configuration for -march and -mtune
  # At the moment, the code is invariably built for the architecture
  # features of the machine it is compiled on.
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall --pedantic" )
  # Avoid super-picky 'nested anon types' warning
  if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang") # Matches regular Clang and AppleClang (XCode)
    # message( STATUS "Detected LLVM C++ compiler" )
    # Remove nonsensical warnings due to unknown pragmas introduced by -pedantic
    # (as pragmas are compiler-specific, unknown ones should be ignored silently anyway)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-nested-anon-types -Wno-unknown-pragmas" )
  endif (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -std=c++11")
endif(UNIX)

if( WIN32 ) # Covers 64-bit Windows as well
  # Macro to determine the Windows version, stolen from
  # http://stackoverflow.com/questions/9742003/platform-detection-in-cmake
  macro(get_win32_winnt_version version)
    if (WIN32 AND CMAKE_SYSTEM_VERSION)
        set(ver ${CMAKE_SYSTEM_VERSION})
        string(REPLACE "." "" ver ${ver})
        string(REGEX REPLACE "([0-9])" "0\\1" ver ${ver})
        set(${version} "0x${ver}")
    endif()
  endmacro()
  get_win32_winnt_version( WIN_VERSION_ID )
  message( STATUS "Window version: " ${WIN_VERSION_ID} )
  add_definitions( -D_WINN32_WINNT=${WIN_VERSION_ID} )
  # Avoid specific compiler warnings of the Visual Studio  compiler.
  add_definitions( -D_SCL_SECURE_NO_WARNINGS ) # STL algorithms with pointers instead of 'real' iterators are perfectly legal.
  # Avoid complaints about STL members in classes with VISR_CORE_LIBRARY_SYMBOL not having DLL interfaces.
  add_definitions( "/wd4251" )
  #
#   ADD_DEFINITIONS( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4005 /wd4996 /wd4251 /nologo" )
   add_definitions( "-D_WIN32_WINNT=0x0601" )


  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3" ) # Increase warning level (/W4 and /Wall generate lots of output not related to coding flaws.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")
endif( WIN32 )

############################################################
# Create universal binaries on MacOS
IF( VISR_SYSTEM_NAME MATCHES "MacOS" )
  OPTION( BUILD_MACOS_UNIVERSAL_BINARIES "Universal (32/64-bit) binaries on MacOS X" ON)
  IF( BUILD_MACOS_UNIVERSAL_BINARIES )
    SET( OSX_UNIVERSAL ON )
    SET( CMAKE_OSX_ARCHITECTURES "i386;x86_64" )
  ENDIF( BUILD_MACOS_UNIVERSAL_BINARIES )
ENDIF( VISR_SYSTEM_NAME MATCHES "MacOS" )

############################################################
# Set the minimum Mac OS version.
if( VISR_SYSTEM_NAME MATCHES "MacOS" )
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")
endif( VISR_SYSTEM_NAME MATCHES "MacOS" )




############################################################
# Set default visibility of shared library symbols to hidden (as on Windows platforms)
SET( CMAKE_C_VISIBILITY_PRESET hidden )
SET( CMAKE_CXX_VISIBILITY_PRESET hidden )

############################################################
# Support for Building Matlab externals

OPTION( BUILD_MATLAB_EXTERNALS "Build Matlab externals" OFF )

IF( BUILD_MATLAB_EXTERNALS )
  FIND_PACKAGE(Matlab REQUIRED)
  IF(MATLAB_FOUND)
    MESSAGE( STATUS "Matlab found." )
  ELSE( MATLAB_FOUND )
    MESSAGE( FATAL_ERROR, "Matlab not found, cancelling compilation." )
  ENDIF( MATLAB_FOUND )
ENDIF( BUILD_MATLAB_EXTERNALS )

################################################################################
# Support for Max/MSP externals
IF( WIN32 OR APPLE )
  OPTION( BUILD_MAX_MSP_EXTERNALS "Build externals for MAX/MSP" OFF )
  IF( BUILD_MAX_MSP_EXTERNALS )
    SET( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{MAX_SDK_ROOT} )
    FIND_PACKAGE( MaxSDK REQUIRED)
    MESSAGE( STATUS "MAX SDK found: " ${MAXSDK_MAINPATH} )
  ENDIF(BUILD_MAX_MSP_EXTERNALS)

  SET( MAXSDK_EXTERNAL_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/max )
ENDIF(  WIN32 OR APPLE )

################################################################################
# Support for Python bindings of selected VISR functionality
OPTION( BUILD_PYTHON_BINDINGS "Python bindings of selected VISR functionality" OFF )

IF( BUILD_PYTHON_BINDINGS )
  # Note: Disabled due to some Python binaries not working (probably due to PYTHONHOME set)
  # if(NOT BUILD_VISR_PYTHON_VERSION)
  #   set( BUILD_VISR_PYTHON_VERSION "" CACHE STRING "Version string to set used Python version" )
  # endif(NOT BUILD_VISR_PYTHON_VERSION)
  # set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4 2.7)
  # find_package(PythonLibsNew ${BUILD_VISR_PYTHON_VERSION} REQUIRED)

  find_package( PythonLibs REQUIRED )

  include_directories( ${PROJECT_SOURCE_DIR}/3rd/include/pybind11 )

  # As we cannot use the PythonLibsNew package due to possible mismatches of the Python library name,
  # we have to set the Python extensions ourselves (using the same variable names as created by
  #  PythonLibsNew.
  if( WIN32 )
    set( PYTHON_MODULE_EXTENSION ".pyd" )
  elseif( APPLE )
    set( PYTHON_MODULE_EXTENSION ".so" )
  else( WIN32 )
    set( PYTHON_MODULE_EXTENSION ${CMAKE_SHARED_LIBRARY_SUFFIX} )
  endif( WIN32 )

  # Enable the pybind11 cmake support
  # TODO: Decide whether it's worth to use that in the future or remove it
  # and the implementation files cmake_modules/pybind11Tools.cmake
  # and cmake_modules/FindPythonLibsNew.cmake
  # Support is activated in the CMakeLists.txt files of the respective
  # Python binding modules.
  include( pybind11Tools )

ENDIF( BUILD_PYTHON_BINDINGS )

###########################################################
# Support for 3rd-party libraries.

# Threads are required wherever we link with boost_threads.
# Note that the dependency is added automatically in most cases, so the 
# omission will go unnoticed very often.
FIND_PACKAGE(Threads REQUIRED)

## Portaudio

# For windows, we ship the includes and libraries.
# TODO: Support system-specific library directories
# TODO: Provide separate libraries for debug and release builds.
IF( WIN32 ) # This includes 64-bit platforms!
  # NOTE: It would be nicer to pass the search path to the FIND_PACKAGE
  # call instead of setting it globally.
  LIST( APPEND CMAKE_INCLUDE_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/include/portaudio) )
  LIST( APPEND CMAKE_LIBRARY_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/lib/${VISR_PLATFORM_NAME}/portaudio) )
 
ENDIF( WIN32 )
FIND_PACKAGE( Portaudio )
INCLUDE_DIRECTORIES( ${PORTAUDIO_INCLUDE_DIR} )

## Jack Audio Connection Kit (JACK)
OPTION( BUILD_USE_JACK "Support the Jack (Jack Audio Connection Kit) audio interface" OFF )
IF( BUILD_USE_JACK )
  # Preliminary support for testing the Jack library on Windows platforms.
  IF( WIN32 )
    SET( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{JACK_ROOT} )
  ENDIF( WIN32 )
  FIND_PACKAGE( Jack REQUIRED)
ENDIF( BUILD_USE_JACK )

## Libsndfile
# On Linux and MacOS, we use the versions provided by the OS (or brew on MacOS)
IF( WIN32 ) # This includes 64-bit platforms!
  # NOTE: It would be nicer to pass the search path to the FIND_PACKAGE
  # call instead of setting it globally.
  LIST( APPEND CMAKE_INCLUDE_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/include/sndfile) )
  LIST( APPEND CMAKE_LIBRARY_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/lib/${VISR_PLATFORM_NAME}/sndfile) )
ENDIF( WIN32 )
FIND_PACKAGE( SndFile REQUIRED )

## KissFFT: Basic FFT support (portable, slow and with a permissive licence)
# This libbrary is provided by 3rd/ for all platforms, because it is no standard library (moreover, heavily tweaked to geet both float and double support)
  LIST( APPEND CMAKE_INCLUDE_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/include/kissfft) )
  LIST( APPEND CMAKE_LIBRARY_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/lib/${VISR_PLATFORM_NAME}/kissfft) )
FIND_PACKAGE( KissFFT REQUIRED )

## FFTW
OPTION( BUILD_USE_FFTW "Use the FFTW library. Do not use for distribution, because this makes the whole project GPL." OFF )
IF( BUILD_USE_FFTW )
  IF( WIN32 ) # This includes 64-bit platforms.
    LIST( APPEND CMAKE_PREFIX_PATH $ENV{FFTW_ROOT} )
  ENDIF( WIN32 )
  FIND_PACKAGE( FFTW REQUIRED )
ENDIF( BUILD_USE_FFTW )

# IPP
OPTION( BUILD_USE_IPP "Use the Intel IPP for fast arithmetic functions and trasnforms. Set the environment variable IPP_ROOT accordingly." OFF )
IF( BUILD_USE_IPP )
  # Use the variable IPP_ROOT to point to the base directory of the library.
  FIND_PACKAGE( IPP REQUIRED )
ENDIF( BUILD_USE_IPP )

## FFTS (Fastest Fourier Transform in the south)
# Because this is a non-standard library, we provide it for all platforms.
# TODO: Consider making this library optional
LIST( APPEND CMAKE_INCLUDE_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/include/ffts) )
LIST( APPEND CMAKE_LIBRARY_PATH TO_CMAKE_PATH(${PROJECT_SOURCE_DIR}/3rd/lib/${VISR_PLATFORM_NAME}/ffts) )
FIND_PACKAGE( FFTS REQUIRED )

## Boost support
# On Linux and Windows, the boost libraries provided by the OS image (or brew on MacOS)
# should be found automatically.
# Otherwise (in particular for Windows platforms), an environment variable
# BOOST_ROOT should hold the path to the boost root directory.

OPTION( Boost_USE_STATIC_LIBS OFF )

# For the moment, we support only dynamic linking of boost.
# In this case, this line is required on Windows to prevent undefined symbols from boost::program_options

IF( NOT Boost_USE_STATIC_LIBS )
  ADD_DEFINITIONS("-DBOOST_ALL_DYN_LINK")
ENDIF( NOT Boost_USE_STATIC_LIBS )

# Avoid problems with the auto-link feature on Windows platforms
ADD_DEFINITIONS("-DBOOST_ALL_NO_LIB")

# Define the boost libraries that must be present.
# In boost 1.59 on Windows, chrono is an implicit dependency of boost_thread. So in order to locate the runtime DLL for packaging, we mus add this library here.
SET( VISR_BOOST_LIBRARIES chrono date_time filesystem program_options system thread unit_test_framework regex timer)
# Note that boost regex is used only with GCC <= 4.8 on Linux because of the faulty std::regex implementation in these GCC versions.
# However, we do not exclude boost::regex here because of the number of places it is handled in the build structure (especially packaging)

# Note: The minimum boost requirement is arbitrarily chosen at the moment. 
# Todo: Replace by a substantiated version requirement.
FIND_PACKAGE( Boost 1.50 REQUIRED COMPONENTS ${VISR_BOOST_LIBRARIES} )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )
 
#WORKING
#add_custom_target(comma ALL COMMAND /bin/sh /Users/gc1y17/VISR/visr/cmake_modules/postscript.sh)
#add_dependencies(comma  rcl)
############################################################
# Copy 3rd party libraries and set RPATH for MacOs
if( VISR_SYSTEM_NAME MATCHES "MacOS" )

  FIND_PACKAGE(Flac REQUIRED)
  FIND_PACKAGE(OggVorbis REQUIRED)

set (EXECUTE_COMMAND mkdir -p ${PROJECT_BINARY_DIR}/3rd)
execute_process(COMMAND ${EXECUTE_COMMAND} RESULT_VARIABLE rv)

function(fix_rpath libpath)
	get_filename_component(libname ${${libpath}} NAME)

	set (EXECUTE_COMMAND  cp ${${libpath}} ${PROJECT_BINARY_DIR}/3rd/${libname})
	execute_process(COMMAND ${EXECUTE_COMMAND} RESULT_VARIABLE rv)

	SET( ${libpath} ${PROJECT_BINARY_DIR}/3rd/${libname})
	SET( ${libpath} ${PROJECT_BINARY_DIR}/3rd/${libname} PARENT_SCOPE ) 

	set (EXECUTE_COMMAND  chmod 777 ${PROJECT_BINARY_DIR}/3rd/${libname})
	execute_process(COMMAND ${EXECUTE_COMMAND} RESULT_VARIABLE rv)

	set (EXECUTE_COMMAND install_name_tool -id @rpath/${libname} ${${libpath}})
	execute_process(COMMAND ${EXECUTE_COMMAND} RESULT_VARIABLE rv)

endfunction()

function(fix_dependencies_of_3rdparty depname libpath)

	set (EXECUTE_COMMAND bash "-c" "${CMAKE_SOURCE_DIR}/cmake_modules/./change_dependency_installname.sh ${depname} ${libpath}" )

	execute_process(COMMAND ${EXECUTE_COMMAND} OUTPUT_VARIABLE rv)
	#message( STATUS "output "${depname} ": " ${rv} )

endfunction()


SET( FIX_LIBRARIES FLAC_LIBRARY OGG_LIBRARY VORBIS_LIBRARY VORBISENC_LIBRARY SNDFILE_LIBRARY PORTAUDIO_LIBRARIES Boost_SYSTEM_LIBRARY Boost_THREAD_LIBRARY Boost_CHRONO_LIBRARY Boost_DATE_TIME_LIBRARY Boost_FILESYSTEM_LIBRARY Boost_PROGRAM_OPTIONS_LIBRARY Boost_UNIT_TEST_FRAMEWORK_LIBRARY Boost_REGEX_LIBRARY Boost_TIMER_LIBRARY)

foreach(v IN LISTS FIX_LIBRARIES)
    fix_rpath(${v})
endforeach()

get_filename_component(FLAC_LIBRARY_NAME ${FLAC_LIBRARY} NAME_WE)
get_filename_component(OGG_LIBRARY_NAME ${OGG_LIBRARY} NAME_WE)
get_filename_component(VORBIS_LIBRARY_NAME ${VORBIS_LIBRARY} NAME_WE)
get_filename_component(VORBISENC_LIBRARY_NAME ${VORBISENC_LIBRARY} NAME_WE)
get_filename_component(Boost_SYSTEM_LIBRARY_NAME ${Boost_SYSTEM_LIBRARY} NAME_WE)
get_filename_component(Boost_TIMER_LIBRARY_NAME ${Boost_TIMER_LIBRARY} NAME_WE)
get_filename_component(Boost_CHRONO_LIBRARY_NAME ${Boost_CHRONO_LIBRARY} NAME_WE)


SET(BOOST_SYSTEM_DEPENDENT ${Boost_FILESYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_CHRONO_LIBRARY} ${Boost_TIMER_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
foreach(v IN LISTS BOOST_SYSTEM_DEPENDENT)
    fix_dependencies_of_3rdparty(${Boost_SYSTEM_LIBRARY_NAME} ${v})
endforeach()


fix_dependencies_of_3rdparty(${Boost_CHRONO_LIBRARY_NAME} ${Boost_TIMER_LIBRARY})
fix_dependencies_of_3rdparty(${Boost_TIMER_LIBRARY_NAME} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

fix_dependencies_of_3rdparty(${FLAC_LIBRARY_NAME} ${SNDFILE_LIBRARY})
fix_dependencies_of_3rdparty(${VORBIS_LIBRARY_NAME} ${SNDFILE_LIBRARY})
fix_dependencies_of_3rdparty(${VORBIS_LIBRARY_NAME} ${VORBISENC_LIBRARY})
fix_dependencies_of_3rdparty(${VORBISENC_LIBRARY_NAME} ${SNDFILE_LIBRARY})

fix_dependencies_of_3rdparty(${OGG_LIBRARY_NAME} ${SNDFILE_LIBRARY})
fix_dependencies_of_3rdparty(${OGG_LIBRARY_NAME} ${VORBIS_LIBRARY})
fix_dependencies_of_3rdparty(${OGG_LIBRARY_NAME} ${VORBISENC_LIBRARY})


endif( VISR_SYSTEM_NAME MATCHES "MacOS" )

################################################################################
# Set the install directories of special output types.
# For Linux this adjusts the path to put the items under /usr/share/visr

if( VISR_SYSTEM_NAME MATCHES "Linux" )
  set( VISR_TOPLEVEL_INSTALL_DIRECTORY "share/visr" ) # Directory under CMAKE_INSTALL_PREFIX where visr-specific content is placed.
else( VISR_SYSTEM_NAME MATCHES "Linux" )
  set( VISR_TOPLEVEL_INSTALL_DIRECTORY "./" ) # CMAKE_INSTALL_PREFIX is already OK
endif( VISR_SYSTEM_NAME MATCHES "Linux" )

set( PYTHON_MODULE_INSTALL_DIRECTORY "${VISR_TOPLEVEL_INSTALL_DIRECTORY}/python" )
set( DOCUMENTATION_INSTALL_DIRECTORY "${VISR_TOPLEVEL_INSTALL_DIRECTORY}/doc" )


################################################################################
# Actual software components (subprojects) to be built are
# contained in the src/ directory
# TODO: This is a quite hidden location for setting the subdirectories and subprojects.
# Restructure the main build file in order to make this line more prominent.
ADD_SUBDIRECTORY( src )

################################################################################
## Support for building Doxygen documentation
## Taken from: http://mementocodex.wordpress.com/2013/01/19/how-to-generate-code-documentation-with-doxygen-and-cmake-a-slightly-improved-approach/

option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ON)
IF(BUILD_DOCUMENTATION) 
  FIND_PACKAGE(Doxygen)
  IF(NOT DOXYGEN_FOUND)
    MESSAGE(FATAL_ERROR
      "Doxygen is needed to build the documentation.")
  ENDIF()
 
  SET( doxyfile_in          ${PROJECT_SOURCE_DIR}/doc/src/Doxyfile.in )
  SET( doxyfile             ${PROJECT_BINARY_DIR}/doc/Doxyfile )
  SET( doxy_html_index_file ${PROJECT_BINARY_DIR}/doc/html/index.html )
  SET( doxy_output_root     ${PROJECT_BINARY_DIR}/doc ) # Pasted into Doxyfile.in
  SET( doxy_input           ${PROJECT_SOURCE_DIR}/src ) # Pasted into Doxyfile.in
#  SET( doxy_extra_files     ${PROJECT_SOURCE_DIR}/doc/src/mainpage.dox    ) # Pasted into Doxyfile.in
 
  CONFIGURE_FILE( ${doxyfile_in} ${doxyfile} @ONLY )
 
  ADD_CUSTOM_COMMAND( OUTPUT ${doxy_html_index_file}
                      COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
                      # The following should be ${doxyfile} only but it
                      # will break the dependency.
                      # The optimal solution would be creating a 
                      # custom_command for ${doxyfile} generation
                      # but I still have to figure out how...
                      MAIN_DEPENDENCY ${doxyfile} ${doxyfile_in}
                      DEPENDS ${doxy_extra_files} # TODO <AF> Decide whether the subprojects should be added as dependency.
                      COMMENT "Generating HTML documentation")
 
  ADD_CUSTOM_TARGET( doc ALL DEPENDS ${doxy_html_index_file} )
 
  INSTALL( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html DESTINATION ${DOCUMENTATION_INSTALL_DIRECTORY} )

ENDIF()

# Instructions for packaging are held in a separate file.
INCLUDE( PackageConfig )

