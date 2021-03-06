# Copyright Institute of Sound and Vibration Research - All rights reserved

# Top-level build configuration for project VISceRal

# 3.0.0+ needed for export(EXPORT) command in src/CMakeLists.txt
# >= 3.1 for CMAKE_CXX_STANDARD
CMAKE_MINIMUM_REQUIRED (VERSION 3.1.0)

# Avoid warnings about the CXX_VISIBILITY_PRESET set for all target types.
if( POLICY CMP0063 ) # (only if the CMake version supports this policy
  CMAKE_POLICY(SET CMP0063 NEW)
endif( POLICY CMP0063 )
# Policy for supporting <package>_ROOT CMake or environment variables in find_package() (CMake >= 3.12 only)
if( POLICY CMP0074 )
  CMAKE_POLICY(SET CMP0074 NEW)
endif( POLICY CMP0074 )

# The name of our top-level project.
PROJECT( VISR CXX C )

# Define central version numbers for the project
# TODO: Decide whether to move that to an external CMake file .
set( VISR_MAJOR_VERSION 0 )
set( VISR_MINOR_VERSION 12 )
set( VISR_PATCH_VERSION 2 )

add_definitions( -DVISR_MAJOR_VERSION=${VISR_MAJOR_VERSION} )
add_definitions( -DVISR_MINOR_VERSION=${VISR_MINOR_VERSION} )
add_definitions( -DVISR_PATCH_VERSION=${VISR_PATCH_VERSION} )

# Add support for unit tests
INCLUDE( CTest )

## Set C++ standard used. 

# Enable C++ 11 support
# This requires CMake >= 3.1
# TODO: Check when we can increase the requirement to C++14
set( CMAKE_CXX_STANDARD 11 )

# Treat the above standard as a hard requirement rather than 'decaying' to a
# previous standard if the requirments aren' met
set( CMAKE_CXX_STANDARD_REQUIRED ON )

# Disable compiler-specific extensions (e.g., GNU extensions)
set( CMAKE_CXX_EXTENSIONS OFF )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake_modules)

# Default path for static libraries
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/staticlib )

# Copy the executables to the binary directory
SET( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )

# Default destination for shared libraries (not DLLs)
SET( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib )

# Default path for DLLs on windows
SET( RUNTIME_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib )

set( VISR_VERSIONED_NAME VISR-${VISR_MAJOR_VERSION}.${VISR_MINOR_VERSION}.${VISR_PATCH_VERSION})

#############################################################
# Create a system string.
SET( VISR_SYSTEM_NAME ${CMAKE_SYSTEM_NAME} )

# Mac OS specific settings (mainly rpath handling)
if( VISR_SYSTEM_NAME MATCHES "Darwin" )
  set( VISR_SYSTEM_NAME MacOS ) # Replace CMake's name for MacOS X by a more descriptive one.

  set( CMAKE_INSTALL_PREFIX /Applications/${VISR_VERSIONED_NAME})

  # Activate rpath support
  set( CMAKE_MACOSX_RPATH ON)
  # Use, i.e., don't skip rpaths in the build tree
  set( CMAKE_SKIP_BUILD_RPATH FALSE)
  set( CMAKE_BUILD_WITH_INSTALL_NAME_DIR TRUE)
  # When building, set the rpath to the libraries in the build tree, not to the install tree
  set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
  set( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/3rd;${CMAKE_INSTALL_PREFIX}/lib")
  # Add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
endif( VISR_SYSTEM_NAME MATCHES "Darwin" )

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
  OPTION( BUILD_MACOS_UNIVERSAL_BINARIES "Universal (32/64-bit) binaries on MacOS X" OFF)
  IF( BUILD_MACOS_UNIVERSAL_BINARIES )
    SET( OSX_UNIVERSAL ON )
    SET( CMAKE_OSX_ARCHITECTURES "i386;x86_64" )
  ENDIF( BUILD_MACOS_UNIVERSAL_BINARIES )
ENDIF( VISR_SYSTEM_NAME MATCHES "MacOS" )

############################################################
# Set the minimum Mac OS version.
if( VISR_SYSTEM_NAME MATCHES "MacOS" )
#  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")
 set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9")
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

  # find_package does not detect if the Python include directories are not found.
  # Checking prevents strange error messages later on.
  if( NOT DEFINED PYTHONLIBS_VERSION_STRING )
    message( FATAL_ERROR "Invalid setting for PYTHON_INCLUDE_DIR." )
  endif( NOT DEFINED PYTHONLIBS_VERSION_STRING )

  # As we cannot use the PythonLibsNew package due to possible mismatches of the Python library name,
  # we have to set the Python extensions ourselves (using the same variable names as created by
  # PythonLibsNew.
  # Note: The pybind11Tools mechanism apparently needs this as well.
  if( WIN32 )
    set( PYTHON_MODULE_EXTENSION ".pyd" )
  elseif( APPLE )
    set( PYTHON_MODULE_EXTENSION ".so" )
  else( WIN32 )
    set( PYTHON_MODULE_EXTENSION ${CMAKE_SHARED_LIBRARY_SUFFIX} )
  endif( WIN32 )

  # This variable is used by pybind11Tools
  SET( PYBIND11_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/3rd/include/pybind11 )

  # Enable the pybind11 cmake support
  # This takes, amongst other things, care of providing proper compile/link options.
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
FIND_PACKAGE( Portaudio REQUIRED )

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

# Define a minimum boost version
# Note: This requirement is chosen heuristically at the moment.
# Todo: Replace by a substantiated version requirement.
set( VISR_BOOST_MINIMUM_VERSION 1.50 )

OPTION( Boost_USE_STATIC_LIBS OFF "Whether to use static or shared boost libraries. Note: Static libraries will often not work for targets that are shared libraries themselves (Python packages, DAW plugins, Matlab or MaxMSP externals) because the library code is not position-independent" )

# Define the boost libraries that will be needed in deployed installations.
# In boost 1.59 on Windows, chrono is an implicit dependency of boost_thread. So in order to locate the runtime DLL for packaging, we mus add this library here.
# Note that unit_test_framework is excluded here because it's only used in local builds.
SET( VISR_BOOST_LIBRARIES chrono date_time filesystem program_options system thread regex timer atomic )
# Note that boost regex is used only with GCC <= 4.8 on Linux because of the faulty std::regex implementation in these GCC versions.
# However, we do not exclude boost::regex here because of the number of places it is handled in the build structure (especially packaging)

FIND_PACKAGE( Boost ${VISR_BOOST_MINIMUM_VERSION} REQUIRED COMPONENTS ${VISR_BOOST_LIBRARIES} unit_test_framework )
 
############################################################
# Copy 3rd party libraries and set RPATH for MacOs
if( VISR_SYSTEM_NAME MATCHES "MacOS" )
  find_package(Flac REQUIRED)
  find_package(OggVorbis REQUIRED)
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
# Adjust the location of 3rd-party libraries on Windows and Mac OS:
# - On Windows, we need to add the locations of all non-standard 3rd-party shared
#   libraries to the %PATH% variable in order to execute built binaries (e.g.,
#   Python externals or unit tests).
# - On Mac OS this is also necessary (depending on the RPATH setting used while
#   building). In addition, we need to adjust the rpath settings of the 3rd-party
#   libraries itself, which implies that they need to be copied.

# Define the location of the 3rd-party runtime libraries (Shared libraries or DLLs)
set( VISR_BUILD_3RD_PARTY_RUNTIME_LIBRARY_DIR ${CMAKE_BINARY_DIR}/3rd )

# NOTE: This must happen before the add_subdirectory( src ) directive.
if( (VISR_SYSTEM_NAME MATCHES "MacOS") OR (VISR_SYSTEM_NAME MATCHES "Windows") )
  include( Adjust3rdPartyLibraries )
endif( (VISR_SYSTEM_NAME MATCHES "MacOS") OR (VISR_SYSTEM_NAME MATCHES "Windows") )

################################################################################
# Actual software components (subprojects) to be built are
# contained in the src/ directory
add_subdirectory( src )

option(BUILD_DOCUMENTATION "Build the API and user documentation (requires Sphinx, breathe, and Doxygen)" ON)
if(BUILD_DOCUMENTATION)
  add_subdirectory(doc)
endif()

# Instructions for packaging are held in a separate file.
include( PackageConfig )
