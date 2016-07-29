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

namespace
{
// create a helper function in an unnamed namespace
  std::vector<std::size_t> indexRange( std::size_t startIdx, std::size_t endIdx )
  {
    if( endIdx < startIdx )
    {
      return std::vector<std::size_t>();
    }
    std::size_t const vecLength( endIdx - startIdx + 1 );
    std::vector < std::size_t> ret( vecLength );
    std::generate( ret.begin(), ret.end(), [&] { return startIdx++; } );
    return ret;
  }
}

SignalFlow::SignalFlow( ril::SignalFlowContext & context,
                        char const * name,
                        ril::CompositeComponent * parent,
                        std::size_t numberOfInputs,
                        std::size_t numberOfOutputs,
                        std::size_t interpolationPeriod,
                        std::string const & configFile,
                        std::size_t udpPort )
 : ril::CompositeComponent( context, name, parent )
 , cNumberOfInputs( numberOfInputs )
 , cNumberOfOutputs( numberOfOutputs )
 , cInterpolationSteps( interpolationPeriod )
 , mConfigFileName( configFile )
 , mNetworkPort( udpPort )
 , mSceneReceiver( context, "SceneReceiver", this )
 , mSceneDecoder( context, "SceneDecoder", this )
 , mSceneEncoder( context, "SceneEncoder", this )
 , mSceneSender( context, "SceneSender", this )
 , mGainCalculator( context, "VbapGainCalculator", this )
 , mMatrix( context, "GainMatrix", this )
{
}

SignalFlow::~SignalFlow( )
{
}
 
/*virtual*/ void 
SignalFlow::process()
{
  mSceneReceiver.process();
  mSceneDecoder.process();
  mGainCalculator.process();
  mMatrix.process();
  //mSceneEncoder.process();
  //mSceneSender.process();
}

/*virtual*/ void
SignalFlow::setup()
{
  panning::LoudspeakerArray loudspeakerArray;
  // As long as we have two different config formats, we decide based on the file extention.
  std::string::size_type lastDotIdx = mConfigFileName.rfind( '.' );
  std::string const configfileExtension = lastDotIdx == std::string::npos ? std::string( ) : mConfigFileName.substr( lastDotIdx + 1 );
  if( boost::iequals( configfileExtension, std::string( "xml" ) ) )
  {
    loudspeakerArray.loadXml( mConfigFileName );
  }
  else
  {
    FILE* hFile = fopen( mConfigFileName.c_str( ), "r" );
    if( loudspeakerArray.load( hFile ) < 0 )
    {
      throw std::invalid_argument( "Error while parsing the loudspeaker array configuration file \""
        + mConfigFileName + "\"." );
    }
  }

  // Initialise and configure audio components

  mSceneReceiver.setup( mNetworkPort, rcl::UdpReceiver::Mode::Asynchronous );
  mSceneDecoder.setup();
  mGainCalculator.setup( cNumberOfInputs, loudspeakerArray );
  mMatrix.setup( cNumberOfInputs, cNumberOfOutputs, cInterpolationSteps, 1.0f );

  mSceneEncoder.setup();
  mSceneSender.setup( 9998, "152.78.243.62", 9999, rcl::UdpSender::Mode::Asynchronous );

#if 0
  initCommArea( cNumberOfInputs + cNumberOfOutputs, period( ), ril::cVectorAlignmentSamples );

  // Set the indices for communicating the signals from and to the outside world.
  std::vector<ril::AudioPort::SignalIndexType> captureIndices = indexRange( 0, cNumberOfInputs - 1 );
  std::vector<ril::AudioPort::SignalIndexType> playbackIndices = indexRange( cNumberOfInputs, cNumberOfInputs + cNumberOfOutputs - 1 );

  // connect the ports
  assignCommunicationIndices( "GainMatrix", "in", captureIndices );
  assignCommunicationIndices( "GainMatrix", "out", playbackIndices );

  assignCaptureIndices( &captureIndices[0], captureIndices.size() );
  assignPlaybackIndices( &playbackIndices[0], playbackIndices.size( ) );

  connectParameterPorts( "SceneReceiver", "messageOutput", "SceneDecoder", "datagramInput" );
  connectParameterPorts( "SceneDecoder", "objectVectorOutput", "VbapGainCalculator", "objectVectorInput" );
  connectParameterPorts( "VbapGainCalculator", "gainOutput", "GainMatrix", "gainInput" );

  initialiseParameterInfrastructure();

  // should not be done here, but in AudioSignalFlow where this method is called.
  setInitialised( true );
#endif
}

} // namespace scene_decoder
} // namespace apps
} // namespace visr
