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

void LateReverbFilterCalculator::process( SubBandMessageQueue & subBandLevels,
  LateFilterMassageQueue & lateFilters )
{
  while( not subBandLevels.empty() )
  {
#if 0
    SubBandMessageQueue::MessageType const & val = subBandLevels.nextElement();
    if( val.first >= mNumberOfFilters )
    {
      throw std::out_of_range( "LateReverbFilterCalculator: Object index out of range." );
    }
    if( val.second.size() != mNumberOfSubBands )
    {
      throw std::invalid_argument( "LateReverbFilterCalculator: The subband level specification has a wrong number of elements." );
    }
    std::vector<ril::SampleType> newFilter( mFilterLength );
    calculateFIR( val.first, val.second, newFilter );
    lateFilters.enqueue( std::make_pair( val.first, newFilter ) );
    subBandLevels.popNextElement();
#endif
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
