/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "delay_vector.hpp"

#include <algorithm>
#include <vector>

namespace visr
{
namespace signalflows
{

DelayVector::DelayVector( SignalFlowContext const & context,
                          const char * name,
                          CompositeComponent * parent, 
                          std::size_t numberOfChannels,
                          std::size_t interpolationPeriod,
                          char const * interpolationMethod )
  : CompositeComponent( context, "", parent )
  , mDelay( context, "DelayVector", this )
  , mInput( "input", *this )
  , mOutput( "output", *this )
  , mGainInput( "globalGainInput", *this, pml::VectorParameterConfig(numberOfChannels) )
  , mDelayInput( "globalDelayInput", *this, pml::VectorParameterConfig( numberOfChannels ) )
{
  // Initialise and configure audio components
  mDelay.setup( numberOfChannels, interpolationPeriod, 0.02f, interpolationMethod,
                rcl::DelayVector::MethodDelayPolicy::Limit, 
                rcl::DelayVector::ControlPortConfig::All, 
                0.0f, 1.0f );
  mInput.setWidth( numberOfChannels );
  mOutput.setWidth( numberOfChannels );

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
