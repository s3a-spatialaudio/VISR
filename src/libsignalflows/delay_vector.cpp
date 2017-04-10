/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "delay_vector.hpp"

#include <algorithm>
#include <vector>

namespace visr
{
namespace signalflows
{

DelayVector::DelayVector( SignalFlowContext & context,
                          const char * name,
                          CompositeComponent * parent, 
                          std::size_t numberOfChannels,
                          std::size_t interpolationPeriod,
                          rcl::DelayVector::InterpolationType interpolationMethod )
  : CompositeComponent( context, "", parent )
  , cNumberOfChannels( numberOfChannels )
  , cInterpolationSteps( interpolationPeriod )
  , cInterpolationMethod( interpolationMethod )
  , mDelay( context, "DelayVector", this )
  , mInput( "input", *this )
  , mOutput( "output", *this )
  , mGainInput( "globalGainInput", *this, pml::VectorParameterConfig(numberOfChannels) )
  , mDelayInput( "globalDelayInput", *this, pml::VectorParameterConfig( numberOfChannels ) )
{
  // Initialise and configure audio components
  mDelay.setup( cNumberOfChannels, cInterpolationSteps,
    0.02f, cInterpolationMethod,
    0.0f, 1.0f );

  mInput.setWidth( cNumberOfChannels );
  mOutput.setWidth( cNumberOfChannels );

  audioConnection( mInput, mDelay.audioPort("in") );
  audioConnection( mDelay.audioPort( "out" ), mOutput );

  parameterConnection( mGainInput, mDelay.parameterPort("gainInput") );
  parameterConnection( mDelayInput, mDelay.parameterPort( "delayInput" ) );
}

DelayVector::~DelayVector()
{
}

} // namespace signalflows
} // namespace visr
