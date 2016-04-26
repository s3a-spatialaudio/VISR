/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MEXSUPPORT_EXPORT_SYMBOL_HPP_INCLUDED
#define VISR_MEXSUPPORT_EXPORT_SYMBOL_HPP_INCLUDED

/**
 * Export a symbol or a class defined in a shared library/module.
 * This is used to control the exported interface of librariesand make it platform-independent
 * despite the different default visibilities of Windows and Unix-based platforms.
 * This assumes that the default visibility on Unix platforms is changed to hidden (as currently
 * done in the top-level makefile.
 * @todo Make this a general library utility (not restricted to mex)
 */
#ifdef _MSC_VER
#define VISR_MEXSUPPORT_EXPORT_SYMBOL __declspec(dllexport)
#else
#define VISR_MEXSUPPORT_EXPORT_SYMBOL __attribute__((__visibility__("default")))
#endif

#endif // VISR_MEXSUPPORT_EXPORT_SYMBOL_HPP_INCLUDED

