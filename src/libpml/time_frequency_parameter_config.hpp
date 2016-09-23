/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_TIME_FREQUENCY_PARAMETER_CONFIG_HPP_INCLUDED
#define VISR_PML_TIME_FREQUENCY_PARAMETER_CONFIG_HPP_INCLUDED

#include <libril/parameter_config_base.hpp>

#include <initializer_list>
#include <istream>

namespace visr
{
namespace pml
{

/**
 */
class TimeFrequencyParameterConfig: public ril::ParameterConfigBase
{
public:
  TimeFrequencyParameterConfig( std::size_t dftSize, 
                                std::size_t hopSize,
                                std::size_t numberOfChannels,
                                std::size_t numberOfDftSamples );

  virtual ~TimeFrequencyParameterConfig();

  bool compare( ril::ParameterConfigBase const & rhs ) const override;

  bool compare( TimeFrequencyParameterConfig const & rhs ) const;

  std::size_t dftSize() const { return mDftSize; }

  std::size_t hopSize() const { return mHopSize; }

  std::size_t numberOfChannels() const { return mNumberOfChannels; }

  std::size_t numberOfDftSamples() const { return mNumberOfDftSamples; }
private:
  std::size_t const mDftSize;

  std::size_t const mHopSize;

  std::size_t const mNumberOfDftSamples;

  std::size_t const mNumberOfChannels;
};

} // namespace pml
} // namespace visr


#endif // VISR_PML_TIME_FREQUENCY_PARAMETER_CONFIG_HPP_INCLUDED
