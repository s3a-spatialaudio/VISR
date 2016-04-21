/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#include <libobjectmodel/point_source_with_reverb.hpp>

#include <algorithm>
#include <utility> // for std::make_pair
#include <vector>

namespace visr
{
namespace mex
{
namespace late_reverb_filter_calculator
{

SignalFlow::SignalFlow( ril::SampleType lateReflectionLengthSeconds,
                        std::size_t numLateReflectionSubBandLevels,
                        std::size_t period,
                        ril::SamplingFrequencyType samplingFrequency )
 : AudioSignalFlow( period, samplingFrequency )
 , mDecoder( *this, "Decoder" )
 , mCalc( *this, "Calculator" )
{
  // Initialise and configure audio components
  mDecoder.setup( );
  mCalc.setup( 1, /* One object only. */
               lateReflectionLengthSeconds,
               numLateReflectionSubBandLevels );


  // should not be done here, but in AudioSignalFlow where this method is called.
  setInitialised( true );
}

SignalFlow::~SignalFlow( )
{
}
 
/*virtual*/ void 
SignalFlow::process( std::string const & objectVector,
                     rcl::LateReverbFilterCalculator::LateFilterMessageQueue & outputQueue )
{
  mMessages.enqueue( objectVector );
  mDecoder.process( mMessages, mObjectVector );

  // For the the moment, extract the Reverb objects here. Later, we might use a ReverbParameterCalculator.
  for( objectmodel::ObjectVector::const_iterator objIt( mObjectVector.begin() );
       objIt != mObjectVector.end(); ++objIt )
  {
    if( objIt->second->type() != objectmodel::ObjectTypeId::PointSourceWithReverb )
    {
      throw std::runtime_error( "All objects must be of type \"PointSourceWithReverb\"" );
    }
    objectmodel::PointSourceWithReverb const & rsao = dynamic_cast<objectmodel::PointSourceWithReverb const &>(*(objIt->second));
    mLateObjects.enqueue( std::make_pair(0,rsao.lateReverb()) );
  }
  mCalc.process( mLateObjects, outputQueue );
}

} // namespace late_reverb_filter_calculator
} // namespace mex
} // namespace visr
