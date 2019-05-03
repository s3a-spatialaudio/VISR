# - Try to find jack-2.6
# Once done this will define
#
#  JACK_FOUND - system has jack
#  JACK_INCLUDE_DIRS - the jack include directory
#  JACK_LIBRARIES - Link these to use jack
#  JACK_DEFINITIONS - Compiler switches required for using jack
#
# Taken from 
# http://gnuradio.org/redmine/projects/gnuradio/repository/changes/cmake/Modules/FindJack.cmake
#
# License:
# 
#  Copyright (c) 2008 Andreas Schneider <mail@cynapses.org>
#  Modified for other libraries by Lasse Kärkkäinen <tronic>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Content of COPYING-CMAKE-SCRIPTS:
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products 
#    derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

if (JACK_LIBRARIES AND JACK_INCLUDE_DIRS)
  # in cache already
  set(JACK_FOUND TRUE)
else (JACK_LIBRARIES AND JACK_INCLUDE_DIRS)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  if (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
    include(UsePkgConfig)
    pkgconfig(jack _JACK_INCLUDEDIR _JACK_LIBDIR _JACK_LDFLAGS _JACK_CFLAGS)
  else (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
    find_package(PkgConfig)
    if (PKG_CONFIG_FOUND)
      pkg_check_modules(_JACK jack)
    endif (PKG_CONFIG_FOUND)
  endif (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
  find_path(JACK_INCLUDE_DIR
    NAMES
      jack/jack.h
    PATHS
      $ENV{JACK_ROOT}/includes
      ${_JACK_INCLUDEDIR}
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )

  find_library(JACK_LIBRARY
    NAMES
      jack
      libjack64 # Name used in the windows distribution.
      jack64    # Name used in the windows distribution.
    PATHS
      $ENV{JACK_ROOT}/lib
      ${_JACK_LIBDIR}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )
  if (JACK_LIBRARY AND JACK_INCLUDE_DIR)
    set(JACK_FOUND TRUE)
    set(JACK_INCLUDE_DIRS
      ${JACK_INCLUDE_DIR}
    )
    set(JACK_LIBRARIES
      ${JACK_LIBRARIES}
      ${JACK_LIBRARY}
    )
  endif (JACK_LIBRARY AND JACK_INCLUDE_DIR)
  if (JACK_FOUND)
    if (NOT JACK_FIND_QUIETLY)
      message(STATUS "Found jack: ${JACK_LIBRARY}")
    endif (NOT JACK_FIND_QUIETLY)
  else (JACK_FOUND)
    if (JACK_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find JACK")
    endif (JACK_FIND_REQUIRED)
  endif (JACK_FOUND)

  # show the JACK_INCLUDE_DIRS and JACK_LIBRARIES variables only in the advanced view
  mark_as_advanced(JACK_INCLUDE_DIRS JACK_LIBRARIES)
endif (JACK_LIBRARIES AND JACK_INCLUDE_DIRS)
