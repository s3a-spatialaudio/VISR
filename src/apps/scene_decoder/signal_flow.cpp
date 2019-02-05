/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#include <libpanning/LoudspeakerArray.h>

#include <boost/algorithm/string.hpp> // case-insensitive string compare

#include <algorithm>
#include <vector>

namespace visr
{
namespace apps
{
namespace scene_decoder
{

SignalFlow::SignalFlow( SignalFlowContext & context,
                        char const * name,
                        CompositeComponent * parent,
                        std::size_t numberOfInputs,
                        std::size_t numberOfOutputs,
                        std::size_t interpolationPeriod,
                        std::string const & configFile,
                        std::size_t udpPort )
 : CompositeComponent( context, name, parent )
 , cNumberOfInputs( numberOfInputs )
 , cNumberOfOutputs( numberOfOutputs )
 , cInterpolationSteps( interpolationPeriod )
 , mNetworkPort( udpPort )
 , mInput( "in", *this )
 , mOutput( "out", *this )
 , mSceneReceiver( context, "SceneReceiver", this, mNetworkPort, rcl::UdpReceiver::Mode::Asynchronous )
 , mSceneDecoder( context, "SceneDecoder", this )
 , mSceneEncoder( context, "SceneEncoder", this )
 , mSceneSender( context, "SceneSender", this, 9998, "152.78.243.62", 9999, rcl::UdpSender::Mode::Asynchronous )
 , mGainCalculator( context, "VbapGainCalculator", this, 
                    numberOfInputs,
                    panning::LoudspeakerArray( configFile ) )
 , mMatrix( context, "GainMatrix", this )
{
   mInput.setWidth( cNumberOfInputs );
   mOutput.setWidth( cNumberOfOutputs );

   // Initialise and configure audio components

   mMatrix.setup( cNumberOfInputs, cNumberOfOutputs, cInterpolationSteps, 1.0f );

   audioConnection( "this", "in", ChannelRange( 0, cNumberOfInputs ), "GainMatrix", "in", ChannelRange( 0, cNumberOfInputs ) );
   audioConnection( "GainMatrix", "out", ChannelRange( 0, cNumberOfOutputs ), "this", "out", ChannelRange( 0, cNumberOfOutputs ) );

   parameterConnection( "SceneReceiver", "messageOutput", "SceneDecoder", "datagramInput" );
   parameterConnection( "SceneDecoder", "objectVectorOutput", "VbapGainCalculator", "objectVectorInput" );
   parameterConnection( "VbapGainCalculator", "gainOutput", "GainMatrix", "gainInput" );

}

 SignalFlow::~SignalFlow( ) = default;
 
} // namespace scene_decoder
} // namespace apps
} // namespace visr
