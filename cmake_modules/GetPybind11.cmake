# Copyright (c) VISR project and contributors - All rights reserved

# Locate pybind11 through find_package(pybind11) in all cases:
# 1. An installed pybind11 package, e.g., provided by a package manager
#    (Conan, vcpkg, conda, the system), the Python distribution
#    (pip install pybind11), or pybind11_DIR / CMAKE_PREFIX_PATH.
#    This step is skipped if PYBIND11_DIR is set.
# 2. Otherwise, pybind11 is installed into the build directory during
#    configuration, either from the source tree given by PYBIND11_DIR or
#    from the release download, and found there.
#
# This file must be include()d (not wrapped in a function), because the pybind11
# package configuration defines variables in the calling scope that are used by
# pybind11_add_module().
#
# Input variables:
#   VISR_PYBIND11_MINIMUM_VERSION  Minimum accepted pybind11 version.
#   VISR_PYBIND11_DOWNLOAD_URL     URL of the pybind11 release archive (.tar.gz)
#                                  used if no installed package is found.
#   VISR_PYBIND11_DOWNLOAD_SHA256  SHA256 hash of that archive.
#   Python_EXECUTABLE              Python interpreter (optional), used to locate
#                                  pybind11 installed into the Python distribution.
#
# Cache variables:
#   PYBIND11_DIR                   Optional pybind11 source tree to be used instead
#                                  of an installed package or the download.
#
# Output variables (in addition to those set by find_package(pybind11)):
#   VISR_PYBIND11_LOCAL            ON if pybind11 has been installed into the build
#                                  directory. In this case it should be installed
#                                  together with VISR.
#   VISR_PYBIND11_LOCAL_PREFIX     Installation prefix of that pybind11.

set(VISR_PYBIND11_LOCAL_DIR ${CMAKE_BINARY_DIR}/_deps)
set(VISR_PYBIND11_LOCAL_PREFIX ${VISR_PYBIND11_LOCAL_DIR}/pybind11-install)
# Records the source of the pybind11 installed into the build directory.
set(VISR_PYBIND11_LOCAL_STAMP ${VISR_PYBIND11_LOCAL_DIR}/pybind11-install.stamp)
# Location of the copy of that pybind11 installed together with VISR.
set(VISR_PYBIND11_INSTALLED_COPY_DIR ${CMAKE_INSTALL_PREFIX}/share/cmake/pybind11)

# Download the pybind11 archive from URL, verify it against SHA256, and extract
# it into DESTINATION. The path of the extracted source tree is returned in
# SOURCE_DIR_VAR.
function(visr_download_pybind11 URL SHA256 DESTINATION SOURCE_DIR_VAR)
  message(STATUS "Downloading pybind11 from ${URL}")
  set(archive ${DESTINATION}.tar.gz)
  file(DOWNLOAD ${URL} ${archive} EXPECTED_HASH SHA256=${SHA256} TLS_VERIFY ON)
  file(MAKE_DIRECTORY ${DESTINATION})
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${archive}
    WORKING_DIRECTORY ${DESTINATION}
    RESULT_VARIABLE result)
  if(result)
    message(FATAL_ERROR "Extracting ${archive} failed.")
  endif()
  # The archive contains a single top-level directory.
  file(GLOB source_dir LIST_DIRECTORIES true ${DESTINATION}/*)
  set(${SOURCE_DIR_VAR} ${source_dir} PARENT_SCOPE)
endfunction()

# Configure the pybind11 source tree SOURCE_DIR in BUILD_DIR with the generator
# settings of the current project, and install it into PREFIX.
function(visr_install_pybind11 SOURCE_DIR BUILD_DIR PREFIX)
  message(STATUS "Installing pybind11 from ${SOURCE_DIR} into ${PREFIX}")
  set(generator_args -G ${CMAKE_GENERATOR})
  if(CMAKE_GENERATOR_PLATFORM)
    list(APPEND generator_args -A ${CMAKE_GENERATOR_PLATFORM})
  endif()
  if(CMAKE_GENERATOR_TOOLSET)
    list(APPEND generator_args -T ${CMAKE_GENERATOR_TOOLSET})
  endif()
  if(CMAKE_GENERATOR_INSTANCE)
    list(APPEND generator_args -DCMAKE_GENERATOR_INSTANCE=${CMAKE_GENERATOR_INSTANCE})
  endif()
  if(CMAKE_MAKE_PROGRAM)
    list(APPEND generator_args -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM})
  endif()
  if(CMAKE_TOOLCHAIN_FILE)
    list(APPEND generator_args -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE})
  endif()
  file(MAKE_DIRECTORY ${BUILD_DIR})
  execute_process(COMMAND ${CMAKE_COMMAND} ${generator_args}
      -DCMAKE_INSTALL_PREFIX=${PREFIX}
      -DPYBIND11_TEST=OFF -DPYBIND11_NOPYTHON=ON
      ${SOURCE_DIR}
    WORKING_DIRECTORY ${BUILD_DIR}
    OUTPUT_QUIET RESULT_VARIABLE result)
  if(NOT result)
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --target install --config Release
      WORKING_DIRECTORY ${BUILD_DIR}
      OUTPUT_QUIET RESULT_VARIABLE result)
  endif()
  if(result)
    message(FATAL_ERROR "Installing pybind11 from ${SOURCE_DIR} failed.")
  endif()
endfunction()

# Instruct pybind11 to use the Python version found by CMake's FindPython module.
set(PYBIND11_FINDPYTHON ON CACHE BOOL "Use CMake's FindPython in pybind11." FORCE)

# Build directories configured with earlier VISR versions have PYBIND11_DIR
# cached, pointing to the formerly included 3rd/pybind11.
if(PYBIND11_DIR AND NOT EXISTS ${PYBIND11_DIR}/CMakeLists.txt)
  unset(PYBIND11_DIR CACHE)
endif()
set(PYBIND11_DIR "" CACHE PATH
  "Optional pybind11 source tree to be used instead of an installed pybind11 package or the download.")

# Re-run the search if a previous run used the pybind11 installed into the build
# directory (or its copy installed together with VISR), e.g., because an
# installed pybind11 package has become available.
string(FIND "${pybind11_DIR}" "${VISR_PYBIND11_LOCAL_PREFIX}" _pybind11_local_pos)
if(_pybind11_local_pos EQUAL 0
   OR pybind11_DIR STREQUAL VISR_PYBIND11_INSTALLED_COPY_DIR)
  unset(pybind11_DIR CACHE)
endif()

# Step 1: Search for an installed pybind11 package.
if(NOT PYBIND11_DIR)
  # Location of the pybind11 package provided by the Python distribution, if any.
  set(_pybind11_python_cmakedir)
  if(Python_EXECUTABLE)
    execute_process(COMMAND ${Python_EXECUTABLE} -m pybind11 --cmakedir
      OUTPUT_VARIABLE _pybind11_python_cmakedir
      OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
  endif()
  # If pybind11 has been installed into the build directory, ignore the copy
  # installed into CMAKE_INSTALL_PREFIX together with VISR.
  set(_pybind11_ignore_path_saved "${CMAKE_IGNORE_PATH}")
  if(EXISTS ${VISR_PYBIND11_LOCAL_STAMP})
    list(APPEND CMAKE_IGNORE_PATH ${VISR_PYBIND11_INSTALLED_COPY_DIR})
  endif()
  find_package(pybind11 ${VISR_PYBIND11_MINIMUM_VERSION} CONFIG QUIET
    HINTS ${_pybind11_python_cmakedir})
  set(CMAKE_IGNORE_PATH "${_pybind11_ignore_path_saved}")
endif()

# Step 2: Install pybind11 into the build directory and find it there.
set(VISR_PYBIND11_LOCAL OFF)
if(NOT pybind11_FOUND)
  set(VISR_PYBIND11_LOCAL ON)
  if(PYBIND11_DIR)
    set(_pybind11_source ${PYBIND11_DIR})
  else()
    set(_pybind11_source ${VISR_PYBIND11_DOWNLOAD_URL})
  endif()
  # The installation is reused in subsequent configure runs as long as the source does not change.
  set(_pybind11_installed_source)
  if(EXISTS ${VISR_PYBIND11_LOCAL_STAMP} AND EXISTS ${VISR_PYBIND11_LOCAL_PREFIX})
    file(READ ${VISR_PYBIND11_LOCAL_STAMP} _pybind11_installed_source)
  endif()
  if(NOT _pybind11_installed_source STREQUAL _pybind11_source)
    file(REMOVE_RECURSE ${VISR_PYBIND11_LOCAL_PREFIX}
      ${VISR_PYBIND11_LOCAL_DIR}/pybind11-src ${VISR_PYBIND11_LOCAL_DIR}/pybind11-build)
    if(PYBIND11_DIR)
      set(_pybind11_source_dir ${PYBIND11_DIR})
    else()
      visr_download_pybind11(${VISR_PYBIND11_DOWNLOAD_URL} ${VISR_PYBIND11_DOWNLOAD_SHA256}
        ${VISR_PYBIND11_LOCAL_DIR}/pybind11-src _pybind11_source_dir)
    endif()
    visr_install_pybind11(${_pybind11_source_dir}
      ${VISR_PYBIND11_LOCAL_DIR}/pybind11-build ${VISR_PYBIND11_LOCAL_PREFIX})
    file(WRITE ${VISR_PYBIND11_LOCAL_STAMP} ${_pybind11_source})
  endif()

  unset(pybind11_DIR CACHE)
  find_package(pybind11 ${VISR_PYBIND11_MINIMUM_VERSION} CONFIG REQUIRED
    NO_DEFAULT_PATH PATHS ${VISR_PYBIND11_LOCAL_PREFIX})
endif()
message(STATUS "Using pybind11 ${pybind11_VERSION} from ${pybind11_DIR}")

unset(_pybind11_local_pos)
unset(_pybind11_python_cmakedir)
unset(_pybind11_ignore_path_saved)
unset(_pybind11_source)
unset(_pybind11_installed_source)
unset(_pybind11_source_dir)
