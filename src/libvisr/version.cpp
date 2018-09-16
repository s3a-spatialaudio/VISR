/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "version.hpp"

#include <algorithm>
#include <iterator>
#include <set>
#include <sstream>

namespace visr
{
namespace version
{

unsigned int major()
{
  return std::stoi( VISR_MAJOR_VERSION );
}

unsigned int minor()
{
  return std::stoi( VISR_MINOR_VERSION );
}

unsigned int patch()
{
  return std::stoi( VISR_PATCH_VERSION );
}

std::string versionString()
{
  std::stringstream stream;
  stream << VISR_MAJOR_VERSION << '.' << VISR_MINOR_VERSION << '.' << VISR_PATCH_VERSION;
  return stream.str();
}


namespace // unnamed
{

using FeatureSet = std::set< std::string>;

// Avoid potential static initialization order fiascos by constructing the static feature list inside a function.
FeatureSet const & featureSet()
{
  static const FeatureSet sFeatures{ "basic", "dynamic" }; // Just for testing
  return sFeatures;
}

}

std::string features()
{
  std::stringstream stream;
  std::copy( featureSet().cbegin(), featureSet().cend(), std::ostream_iterator<std::string>(stream, "," ) );
  return stream.str();
}

bool hasFeature( std::string const & feature )
{
  return featureSet().find( feature ) != featureSet().cend();
}

}
}
