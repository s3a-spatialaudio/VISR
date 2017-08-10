/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_AUDIOINTERFACES_EXPORT_SYMBOLS_HPP_INCLUDED
#define VISR_AUDIOINTERFACES_EXPORT_SYMBOLS_HPP_INCLUDED

/**
 * @file
 *
 */

#ifdef VISR_BUILD_AUDIOINTERFACES_SHARED_LIBRARY

#ifdef _MSC_VER // Windows platform
#ifdef VISR_BUILD_AUDIOINTERFACES_LIBRARY
#define VISR_AUDIOINTERFACES_LIBRARY_SYMBOL __declspec(dllexport)
#else
#define VISR_AUDIOINTERFACES_LIBRARY_SYMBOL __declspec(dllimport)
#endif // #ifdef VISR_BUILD_AUDIOINTERFACES_LIBRARIES
#elif defined(__GNUC__)

// Unix platforms (Linux and Mac OS X)
#define VISR_AUDIOINTERFACES_LIBRARY_SYMBOL __attribute__((visibility("default"))) 

#else
#error "Platform does not support symbol export."
#endif // #ifdef _WIN32

#else // VISR_BUILD_AUDIOINTERFACES_SHARED_LIBRARY

#define VISR_AUDIOINTERFACES_LIBRARY_SYMBOL // expand to empty string

#endif // VISR_BUILD_AUDIOINTERFACES_SHARED_LIBRARY

#endif // #ifndef VISR_AUDIOINTERFACES_EXPORT_SYMBOLS_HPP_INCLUDED
