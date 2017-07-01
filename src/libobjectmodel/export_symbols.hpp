/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_EXPORT_SYMBOLS_HPP_INCLUDED
#define VISR_OBJECTMODEL_EXPORT_SYMBOLS_HPP_INCLUDED

/**
 * @file
 *
 */

// For unknown reasons this definition does not becaome visible here:
#ifdef VISR_BUILD_OBJECTMODEL_SHARED_LIBRARY
#ifdef _MSC_VER // Windows platform
#ifdef VISR_BUILD_OBJECTMODEL_LIBRARY
#define VISR_OBJECTMODEL_LIBRARY_SYMBOL __declspec(dllexport)
#else
#define VISR_OBJECTMODEL_LIBRARY_SYMBOL __declspec(dllimport)
#endif // #ifdef VISR_BUILD_OBJECTMODEL_LIBRARY
#elif defined(__GNUC__)

// Unix platforms (Linux and Mac OS X)
#define VISR_OBJECTMODEL_LIBRARY_SYMBOL __attribute__((visibility("default"))) 

#else
#error "Platform does not support symbol export."
#endif // #ifdef _WIN32

#else // #ifdef VISR_BUILD_OBJECTMODEL_SHARED_LIBRARY
#define VISR_OBJECTMODEL_LIBRARY_SYMBOL // empty
#endif // #ifdef VISR_BUILD_OBJECTMODEL_SHARED_LIBRARY

#endif // #ifndef VISR_OBJECTMODEL_EXPORT_SYMBOLS_HPP_INCLUDED
