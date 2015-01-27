/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "delay_vector.hpp"

#include <ciso646>

namespace visr
{
namespace rcl
{

DelayVector::DelayVector( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mInput( "in", *this )
 , mOutput( "out", *this )
{
}

void DelayVector::setup( std::size_t numberOfChannels, 
                         std::size_t interpolationSteps,
                         SampleType maximumDelaySeconds,
                         SampleType initialDelaySeconds /* = static_cast<SampleType>(1.0) */,
                         SampleType initialGainLinear /* = static_cast<SampleType>(0.0) */ )
{
}

 void DelayVector::setup( std::size_t numberOfChannels,
                          std::size_t interpolationSteps,
                          SampleType maximumDelaySeconds,
                          efl::BasicMatrix< SampleType > const & initialDelaysSeconds,
                          efl::BasicMatrix< SampleType > const & initialGainsLinear )
{

}

void DelayVector::process()
{
}

void DelayVector::setDelayAndGain( efl::BasicMatrix< SampleType > const & newDelays,
                                   efl::BasicMatrix< SampleType > const & newGains )
{
}

void DelayVector::setDelay( efl::BasicMatrix< SampleType > const & newDelays )
{
}

} // namespace rcl
} // namespace visr
