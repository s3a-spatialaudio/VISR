/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_VERSION_HPP_INCLUDED
#define VISR_VERSION_HPP_INCLUDED

#include "export_symbols.hpp"

#include <string>

// Avoid warning with newer glibc versions (>=2.25)
// This might cause problems if code included after this file uses these macros.
#if defined(__GLIBC__) && defined( __GLIBC_MINOR__) && (__GLIBC__ >= 2) && (__GLIBC_MINOR__ >= 25 )
#undef major
#undef minor
#endif

namespace visr
{

/**
 * Namespace containing version number and version name access functions.
 *
 */
namespace version
{

/**
 * Return the major version number as an integer.
 */
VISR_CORE_LIBRARY_SYMBOL unsigned int major();


/**
 * Return the mainor version number as an integer.
 */
VISR_CORE_LIBRARY_SYMBOL unsigned int minor();

/**
 * Return the patch revision number as an integer.
 */
VISR_CORE_LIBRARY_SYMBOL unsigned int patch();

/**
 * Return the software version as a string of the form &lt;major&gt;.&lt;minor&gt;.&lt;patch&gt;
 */
VISR_CORE_LIBRARY_SYMBOL std::string versionString();

/**
 * Return a comma-separated list of additional features.
 */
VISR_CORE_LIBRARY_SYMBOL std::string features();

/**
 * Query whether the software contains the specified additional feature.
 * @param feature Feature id (a string).
 * @return True if the feature is contained in the library, false otherwise.
 */
VISR_CORE_LIBRARY_SYMBOL bool hasFeature( std::string const & feature );

} // namespace version
} // namespace visr

#endif // #ifndef VISR_AUDIO_INPUT_HPP_INCLUDED
