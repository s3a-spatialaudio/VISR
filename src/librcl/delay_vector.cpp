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
                          efl::BasicVector< SampleType > const & initialDelaysSeconds,
                          efl::BasicVector< SampleType > const & initialGainsLinear )
{

}

void DelayVector::process()
{
}

void DelayVector::setDelayAndGain( efl::BasicVector< SampleType > const & newDelays,
                                   efl::BasicVector< SampleType > const & newGains )
{
}

void DelayVector::setDelay( efl::BasicVector< SampleType > const & newDelays )
{
}

void DelayVector::setGain( efl::BasicVector< SampleType > const & newGains )
{
}

} // namespace rcl
} // namespace visr
