/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "time_frequency_parameter.hpp"

#include <libril/constants.hpp>
#include <libril/parameter_factory.hpp>

#include <ciso646>
#include <complex>
#include <fstream>
#include <limits>
#include <regex>
#include <stdexcept>
#include <sstream>
#include <vector>

namespace visr
{
namespace pml
{

template< typename ElementType >
TimeFrequencyParameter<ElementType>::TimeFrequencyParameter( std::size_t alignment /*= 0*/ )
  : mData( alignment )
  , mNumberOfChannels( 0 )
{
  mData.zeroFill();
}

template< typename ElementType >
TimeFrequencyParameter<ElementType>::TimeFrequencyParameter( ParameterConfigBase const & config )
  : TimeFrequencyParameter<ElementType>( dynamic_cast<TimeFrequencyParameterConfig const &>(config))
{

}

template< typename ElementType >
TimeFrequencyParameter<ElementType>::TimeFrequencyParameter( TimeFrequencyParameterConfig const & config )
  : mData( config.numberOfChannels() * config.numberOfDftSamples(), config.dftSize(), cVectorAlignmentSamples )
  , mNumberOfChannels( config.numberOfChannels() )
{

}

template< typename ElementType >
TimeFrequencyParameter<ElementType>::TimeFrequencyParameter( std::size_t dftSize,
                                                             std::size_t numDftSamples,
                                                             std::size_t numChannels,
                                                             size_t alignment /*= 0*/ )
 : mData( numDftSamples * numChannels, dftSize, alignment )
 , mNumberOfChannels( numChannels )
{
  // Zero-initialise
  mData.zeroFill();
}

template< typename ElementType >
TimeFrequencyParameter<ElementType>::TimeFrequencyParameter( TimeFrequencyParameter<ElementType> const & rhs )
  : mData( rhs.mData.numberOfRows(), rhs.mData.numberOfColumns(), rhs.alignment() )
  , mNumberOfChannels( rhs.numberOfChannels() )
{
  mData.copy( rhs.mData );
}

template< typename ElementType >
TimeFrequencyParameter<ElementType>::~TimeFrequencyParameter() = default;

template< typename ElementType >
TimeFrequencyParameter<ElementType>&
TimeFrequencyParameter<ElementType>::operator=( TimeFrequencyParameter<ElementType> const & rhs )
{
  if( &rhs == this )
  {
    return *this;
  }
  mData.resize( rhs.mData.numberOfRows(), rhs.mData.numberOfColumns() );
  mData.copy( rhs.mData );
  mNumberOfChannels = rhs.numberOfChannels();
  return *this;
}

template< typename ElementType >
void TimeFrequencyParameter<ElementType>::
resize( std::size_t dftSize, std::size_t numDftSamples, std::size_t numChannels )
{
  mData.resize( numDftSamples * numChannels, dftSize );
  mData.zeroFill();
  mNumberOfChannels = numChannels;
}

// Explicit instantiations for sample types float and double
template class TimeFrequencyParameter<float>;
template class TimeFrequencyParameter<double>;

} // namespace pml
} // namespace visr
