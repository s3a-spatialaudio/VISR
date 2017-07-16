/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_MEXSUPPORT_EXPORT_SYMBOL_HPP_INCLUDED
#define VISR_MEXSUPPORT_EXPORT_SYMBOL_HPP_INCLUDED

#ifdef _MSC_VER
#define VISR_MEXSUPPORT_EXPORT_SYMBOL
#else
/**
* Export a symbol or a class defined in a shared library/module.
* This is used to control the exported interface of librariesand make it platform-independent
* despite the different default visibilities of Windows and Unix-based platforms.
* This assumes that the default visibility on Unix platforms is changed to hidden (as currently
* done in the top-level makefile.
* @note On Windows, Matlab requires that the export properties are set via an additional .def file. Therefore 
* we need to define this as empty.
*/
#define VISR_MEXSUPPORT_EXPORT_SYMBOL __attribute__((__visibility__("default")))
#endif

#endif // VISR_MEXSUPPORT_EXPORT_SYMBOL_HPP_INCLUDED

