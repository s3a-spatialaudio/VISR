/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_VERSION_HPP_INCLUDED
#define VISR_VERSION_HPP_INCLUDED

#define VISR_API_VERSION_MAJOR 0
#define VISR_API_VERSION_MINOR 9
#define VISR_API_VERSION_PATCH 0

#include <string>

namespace visr
{
namespace version
{

unsigned int major();


unsigned int minor();


unsigned int patch();

std::string versionString();


std::string features();


bool hasFeature( std::string const & feature );

} // namespace version
} // namespace visr

#endif // #ifndef VISR_AUDIO_INPUT_HPP_INCLUDED
