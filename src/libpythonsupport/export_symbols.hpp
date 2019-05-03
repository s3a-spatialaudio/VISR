/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PYTHONSUPPORT_EXPORT_SYMBOLS_HPP_INCLUDED
#define VISR_PYTHONSUPPORT_EXPORT_SYMBOLS_HPP_INCLUDED

/**
 * @file 
 *
 * Define macros for exporting and importing shared library symbols
 * of the pythonsupport library.
 */

#ifndef VISR_PYTHONSUPPORT_STATIC_LIBRARY
#ifdef _MSC_VER // Windows platform
#ifdef VISR_BUILD_PYTHONSUPPORT_LIBRARY
#define VISR_PYTHONSUPPORT_LIBRARY_SYMBOL __declspec(dllexport)
#else
#define VISR_PYTHONSUPPORT_LIBRARY_SYMBOL __declspec(dllimport)
#endif // #ifdef VISR_BUILD_PYTHONSUPPORT_LIBRARY
#elif defined(__GNUC__)

// Unix platforms (Linux and Mac OS X)
#define VISR_PYTHONSUPPORT_LIBRARY_SYMBOL __attribute__((visibility("default"))) 

#else
#error "Platform does not support symbol export."
#endif // #ifdef _WIN32

#else // #ifdef VISR_PYTHONSUPPORT_STATIC_LIBRARY
#define VISR_PYTHONSUPPORT_LIBRARY_SYMBOL // empty
#endif // #ifdef VISR_PYTHONSUPPORT_STATIC_LIBRARY

#endif // #ifndef VISR_PYTHONSUPPORT_EXPORT_SYMBOLS_HPP_INCLUDED
