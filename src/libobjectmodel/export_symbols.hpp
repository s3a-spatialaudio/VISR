/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_EXPORT_SYMBOLS_HPP_INCLUDED
#define VISR_OBJECTMODEL_EXPORT_SYMBOLS_HPP_INCLUDED

/**
 * @file
 *
 */

#ifdef VISR_BUILD_OBJECTMODEL_SHARED_LIBRARY
#ifdef _MSC_VER // Windows platform
#ifdef VISR_BUILD_OBJECTMODEL_LIBRARY
#define VISR_OBJECTMODEL_LIBRARY_SYMBOL __declspec(dllexport)
#define VISR_OBJECTMODEL_PLUGIN_SYMBOL __declspec(dllimport)
#else
#define VISR_OBJECTMODEL_LIBRARY_SYMBOL __declspec(dllimport)
#endif // #ifdef VISR_BUILD_OBJECTMODEL_LIBRARIES
#elif defined(__GNUC__)

// Unix platforms (Linux and Mac OS X)
#define VISR_OBJECTMODEL_LIBRARY_SYMBOL __attribute__((visibility("default"))) 
#define VISR__OBJECTMODEL_PLUGIN_SYMBOL __attribute__((visibility("default")))

#else
#error "Platform does not support symbol export."
#endif // #ifdef _WIN32

#else // #ifdef VISR_BUILD_OBJECTMODEL_SHARED_LIBRARY
#define VISR_OBJECTMODEL_LIBRARY_SYMBOL // empty
#endif // #ifdef VISR_BUILD_OBJECTMODEL_SHARED_LIBRARY

#endif // #ifndef VISR_OBJECTMODEL_EXPORT_SYMBOLS_HPP_INCLUDED
