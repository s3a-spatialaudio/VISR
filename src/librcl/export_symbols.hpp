/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_RCL_EXPORT_SYMBOLS_HPP_INCLUDED
#define VISR_RCL_EXPORT_SYMBOLS_HPP_INCLUDED

/**
 * @file 
 *
 * Define macros for exporting and importing shared library symbols
 * of the rcl library.
 */

#ifndef VISR_RCL_STATIC_LIBRARY
#ifdef _MSC_VER // Windows platform
#ifdef VISR_BUILD_RCL_LIBRARY
#define VISR_RCL_LIBRARY_SYMBOL __declspec(dllexport)
#else
#define VISR_RCL_LIBRARY_SYMBOL __declspec(dllimport)
#endif // #ifdef VISR_BUILD_RCL_LIBRARY
#elif defined(__GNUC__)

// Unix platforms (Linux and Mac OS X)
#define VISR_RCL_LIBRARY_SYMBOL __attribute__((visibility("default"))) 

#else
#error "Platform does not support symbol export."
#endif // #ifdef _WIN32

#else // #ifdef VISR_RCL_STATIC_LIBRARY
#define VISR_RCL_LIBRARY_SYMBOL // empty
#endif // #ifdef VISR_RCL_STATIC_LIBRARY

#endif // #ifndef VISR_RCL_EXPORT_SYMBOLS_HPP_INCLUDED
