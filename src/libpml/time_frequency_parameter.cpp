/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "time_frequency_parameter.hpp"

#include <libril/constants.hpp>

#include <sndfile.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

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
TimeFrequencyParameter<ElementType>::TimeFrequencyParameter( ril::ParameterConfigBase const & config )
  : TimeFrequencyParameter<ElementType>( dynamic_cast<TimeFrequencyParameterConfig const &>(config))
{

}

template< typename ElementType >
TimeFrequencyParameter<ElementType>::TimeFrequencyParameter( TimeFrequencyParameterConfig const & config )
  : mData( config.numberOfChannels() * config.numberOfDftSamples(), config.dftSize(), ril::cVectorAlignmentSamples )
  , mNumberOfChannels( config.numberOfChannels() )
{

}

  /**
  * Construct a parameter matrix with the given dimensions.
  * The matrix is zero-initialised.
  * @param numRows The number of matrix rows.
  * @param numColumns The number of columns.
  * @param alignment The alignment of the data, given in in multiples of the eleement size.
  */
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
TimeFrequencyParameter<ElementType>::TimeFrequencyParameter( std::size_t dftSize,
                                                             std::size_t numDftSamples,
                                                             std::size_t numChannels,
                                                             std::initializer_list<std::complex< ElementType> > const & initMtx,
                                                             std::size_t alignment /*= 0*/ )
  : mData( numDftSamples * numChannels, dftSize, alignment )
  , mNumberOfChannels( numChannels )
{
  // TODO: Copy the initialiser list
}

template< typename ElementType >
TimeFrequencyParameter<ElementType>::TimeFrequencyParameter( TimeFrequencyParameter<ElementType> const & rhs )
  : mData( rhs.mData.numberOfRows(), rhs.mData.numberOfColumns(), rhs.alignment() )
  , mNumberOfChannels( rhs.numberOfChannels() )
{
  mData.copy( rhs.mData );
}

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
