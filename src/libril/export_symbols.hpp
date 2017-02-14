/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_EXPORT_SYMBOLS_HPP_INCLUDED
#define VISR_EXPORT_SYMBOLS_HPP_INCLUDED

/**
 * @file
 *
 */

#ifdef _MSC_VER // Windows platform
#ifdef VISR_BUILD_CORE_LIBRARIES
#define VISR_CORE_LIBRARY_SYMBOL __declspec(dllexport)
#define VISR_PLUGIN_SYMBOL __declspec(dllimport)
#else
#define VISR_CORE_LIBRARY_SYMBOL __declspec(dllimport)
#define VISR_PLUGIN_SYMBOL __declspec(dllexport)
#endif // #ifdef VISR_BUILD_CORE_LIBRARIES
#elif defined(__GNUC__)

// Unix platforms (Linux and Mac OS X)
#define VISR_CORE_LIBRARY_SYMBOL 
#define VISR_PLUGIN_SYMBOL __attribute__((visibility("default")))

#else
#error "Platform does not support symbol export. 
#endif // #ifdef _WIN32



#endif // #ifndef VISR_EXPORT_SYMBOLS_HPP_INCLUDED
