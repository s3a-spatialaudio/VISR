/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "late_reverb_filter_calculator.hpp"

#include <libefl/basic_vector.hpp>

#include <libpml/message_queue.hpp>

#include <ciso646>
#include <cmath>

namespace visr
{
namespace rcl
{

LateReverbFilterCalculator::LateReverbFilterCalculator( ril::AudioSignalFlow& container, char const * name )
 : AtomicComponent( container, name )
 , mSubbandInput( *this, "subbandInput", pml::EmptyParameterConfig() )
 , mFilterOutput( *this, "lateFilterOutput", pml::EmptyParameterConfig( ) )
{
}

LateReverbFilterCalculator::~LateReverbFilterCalculator()
{
}

void LateReverbFilterCalculator::setup( std::size_t numberOfObjects,
                                        ril::SampleType lateReflectionLengthSeconds,
                                        std::size_t numLateReflectionSubBandLevels )
{
  mNumberOfFilters = numberOfObjects;
  mNumberOfSubBands = numLateReflectionSubBandLevels;
  mFilterLength = static_cast<std::size_t>( std::ceil( lateReflectionLengthSeconds * samplingFrequency() ) );
}

void LateReverbFilterCalculator::process()
{
  while( not mSubbandInput.empty() )
  {
    pml::IndexedValueParameter<std::size_t, std::vector<ril::SampleType> > const & val = mSubbandInput.front( );
    std::size_t const idx = val.index();
    if( idx >= mNumberOfFilters )
    {
      throw std::out_of_range( "LateReverbFilterCalculator: Object index out of range." );
    }
    if( val.value().size() != mNumberOfSubBands )
    {
      throw std::invalid_argument( "LateReverbFilterCalculator: The subband level specification has a wrong number of elements." );
    }
    std::vector<ril::SampleType> newFilter( mFilterLength );
    calculateFIR( val.first, val.value(), newFilter );
    mFilterOutput.enqueue( pml::IndexedValueParameter<std::size_t, std::vector<ril::SampleType> >(  ) );
    mSubbandInput.pop();
  }
}

void LateReverbFilterCalculator::calculateFIR( std::size_t objectIdx,
                                               std::vector<ril::SampleType> const & subBandLevels,
                                               std::vector<ril::SampleType> & reverbFilter )
{
  // Do whatever needed to calculate the reverb filter
}


} // namespace rcl
} // namespace visr
