/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_EXPORT_SYMBOLS_HPP_INCLUDED
#define VISR_EXPORT_SYMBOLS_HPP_INCLUDED

/**
 * @file
 *
 * Define macros for exporting and importing shared library symbols
 * of the visr core library.
 */

#ifndef VISR_CORE_STATIC_LIBRARY

#ifdef _MSC_VER // Windows platform
#ifdef VISR_BUILD_CORE_LIBRARY
#define VISR_CORE_LIBRARY_SYMBOL __declspec(dllexport)
#define VISR_PLUGIN_SYMBOL __declspec(dllimport)
#else
// Does create undefined symbols if the library is built statically, therefore the attributes are commented out.
// @note logic add logic to add these attributes only if the core library is built dynamically.
#define VISR_CORE_LIBRARY_SYMBOL __declspec(dllimport)
#define VISR_PLUGIN_SYMBOL __declspec(dllexport)
#endif // #ifdef VISR_BUILD_CORE_LIBRARY
#elif defined(__GNUC__)

// Unix platforms (Linux and Mac OS X)
#define VISR_CORE_LIBRARY_SYMBOL __attribute__((visibility("default"))) 
#define VISR_PLUGIN_SYMBOL __attribute__((visibility("default")))

#else
#error "Platform does not support symbol export."
#endif // #ifdef _WIN32

#else // #ifndef VISR_CORE_STATIC_LIBRARY

#define VISR_CORE_LIBRARY_SYMBOL // define as empty
#define VISR_PLUGIN_SYMBOL // define as empty

#endif // #ifdef VISR_CORE_STATIC_LIBRARY

#endif // #ifndef VISR_EXPORT_SYMBOLS_HPP_INCLUDED
