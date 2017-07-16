/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "time_frequency_parameter_config.hpp"

#include <ciso646>
#include <stdexcept>
#include <typeinfo>
#include <typeindex>

namespace visr
{
namespace pml
{

TimeFrequencyParameterConfig::TimeFrequencyParameterConfig( std::size_t dftSize,
                                                            std::size_t hopSize,
                                                            std::size_t numberOfChannels,
                                                            std::size_t numberOfDftSamples )
 : ParameterConfigBase()
 , mDftSize( dftSize )
 , mHopSize( hopSize )
 , mNumberOfDftSamples( numberOfDftSamples )
 , mNumberOfChannels(numberOfChannels)
{
}

TimeFrequencyParameterConfig::~TimeFrequencyParameterConfig()
{
}

std::unique_ptr< ParameterConfigBase > TimeFrequencyParameterConfig::clone() const
{
  return std::unique_ptr<ParameterConfigBase>( new TimeFrequencyParameterConfig( *this ) );
}

bool TimeFrequencyParameterConfig::compare(ParameterConfigBase const & rhs) const
{
  // maybe move this to the base class.
  if (std::type_index(typeid(rhs)) != std::type_index(typeid(TimeFrequencyParameterConfig)))
  {
	throw std::invalid_argument("Called compare() for incompatible parameter config types");
  }
  return compare(static_cast<TimeFrequencyParameterConfig const &>(rhs));
}

bool TimeFrequencyParameterConfig::compare(TimeFrequencyParameterConfig const & rhs) const
{
  return ( (rhs.dftSize() == dftSize() )
    and (rhs.hopSize() == hopSize() )
    and (rhs.numberOfChannels() == numberOfChannels())
    and (rhs.numberOfDftSamples() == numberOfDftSamples()));
}

} // namespace pml
} // namespace visr
