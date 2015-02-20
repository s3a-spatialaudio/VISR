/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "signal_flow.hpp"

#include <algorithm>
#include <vector>

namespace visr
{
namespace apps
{
namespace baseline_renderer_tracking
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

SignalFlow::SignalFlow( std::size_t numberOfInputs,
  std::size_t numberOfLoudspeakers,
  std::size_t numberOfOutputs,
  pml::SignalRoutingParameter const & outputRouting,
  std::size_t interpolationPeriod,
  std::string const & configFile,
  std::size_t sceneReceiverPort,
  std::size_t kinectPort,
  std::size_t period,
  ril::SamplingFrequencyType samplingFrequency )
 : AudioSignalFlow( period, samplingFrequency )
 , cNumberOfInputs( numberOfInputs )
 , cNumberOfLoudspeakers( numberOfLoudspeakers )
 , cNumberOfOutputs( numberOfOutputs )
 , mOutputRoutings( outputRouting)
 , cInterpolationSteps( interpolationPeriod )
 , cConfigFileName( configFile )
 , cSceneReceiverUdpPort( sceneReceiverPort )
 , cTrackingUdpPort( kinectPort )
 , mSceneReceiver( *this, "SceneReceiver" )
 , mSceneDecoder( *this, "SceneDecoder" )
 , mOutputRouting( *this, "OutputSignalRouting" )
 , mGainCalculator( *this, "VbapGainCalculator" )
 , mMatrix( *this, "GainMatrix" )
 , mListenerCompensation(*this, "ListenerCompensation")
 , mListenerPosition() // use default constructor
 , mSpeakerCompensation(*this, "SpeakerCompensation")
 , mKinectReceiver(*this, "KinectReceiver" )
 , mPositionDecoder( *this, "PositionDecoder" )
 , mTrackingMessages()
 , mCompensationGains(ril::cVectorAlignmentSamples)
 , mCompensationDelays(ril::cVectorAlignmentSamples)
{
}

SignalFlow::~SignalFlow( )
{
}
 
/*virtual*/ void 
SignalFlow::process()
{
  mSceneReceiver.process( mSceneMessages );
  mSceneDecoder.process( mSceneMessages, mObjectVector );
  mKinectReceiver.process(mTrackingMessages);
  mPositionDecoder.process( mTrackingMessages, mListenerPosition );
  mGainCalculator.setListenerPosition( mListenerPosition );
  mGainCalculator.process( mObjectVector, mGainParameters );

  mListenerCompensation.process(mListenerPosition, mCompensationGains, mCompensationDelays);
 
  mMatrix.setGains( mGainParameters );
  mMatrix.process();
  mSpeakerCompensation.setDelayAndGain(mCompensationDelays, mCompensationGains);
  mSpeakerCompensation.process();
  mOutputRouting.process();
}

/*virtual*/ void
SignalFlow::setup()
{
  // Initialise and configure audio components

  mSceneReceiver.setup( cSceneReceiverUdpPort, rcl::UdpReceiver::Mode::Synchronous );
  mSceneDecoder.setup();
  mGainCalculator.setup( cNumberOfInputs, cNumberOfLoudspeakers, cConfigFileName );
  mMatrix.setup( cNumberOfInputs, cNumberOfLoudspeakers, cInterpolationSteps, 0.0f );
  mOutputRouting.setup( cNumberOfLoudspeakers, cNumberOfOutputs, mOutputRoutings );
  mListenerCompensation.setup(cNumberOfLoudspeakers, cConfigFileName);
  ril::SampleType cMaxDelay = 1.0f; // maximum delay (in seconds)
  // We start with a initial gain of 0.0 to suppress transients on startup.
  mSpeakerCompensation.setup(cNumberOfLoudspeakers, period(), cMaxDelay, 0.0f, 0.0f);
  mKinectReceiver.setup(cTrackingUdpPort, rcl::UdpReceiver::Mode::Synchronous);
  mPositionDecoder.setup();

  initCommArea( cNumberOfInputs + 2*cNumberOfLoudspeakers + cNumberOfOutputs, period( ), ril::cVectorAlignmentSamples );

  // connect the ports
  std::vector<ril::AudioPort::SignalIndexType> captureIndices = indexRange( 0, cNumberOfInputs - 1 );

  assignCommunicationIndices( "GainMatrix", "in", captureIndices );

  std::size_t matrixOutStartIdx = cNumberOfInputs;
  std::vector<std::size_t> const matrixOutRange = indexRange( matrixOutStartIdx, matrixOutStartIdx + cNumberOfLoudspeakers - 1 );
  assignCommunicationIndices( "GainMatrix", "out", matrixOutRange );
  assignCommunicationIndices( "SpeakerCompensation", "in", matrixOutRange );

  std::size_t compensationOutStartIdx = cNumberOfInputs + cNumberOfLoudspeakers;
  std::vector<std::size_t> const compensationOutRange = indexRange(compensationOutStartIdx, compensationOutStartIdx + cNumberOfLoudspeakers - 1);
  assignCommunicationIndices("SpeakerCompensation", "out", compensationOutRange);
  assignCommunicationIndices("OutputSignalRouting", "in", compensationOutRange);

  std::size_t routingOutStartIdx = matrixOutStartIdx + 2*cNumberOfLoudspeakers;
  std::vector<std::size_t> const routingOutRange = indexRange( routingOutStartIdx, routingOutStartIdx + cNumberOfOutputs - 1 );
  assignCommunicationIndices( "OutputSignalRouting", "out", routingOutRange );

  mCompensationGains.resize(cNumberOfLoudspeakers);

  mCompensationDelays.resize(cNumberOfLoudspeakers);

  assignCaptureIndices( &captureIndices[0], captureIndices.size() );
  assignPlaybackIndices( &routingOutRange[0], routingOutRange.size() );

  mGainParameters.resize( cNumberOfLoudspeakers, cNumberOfInputs );

  // should not be done here, but in AudioSignalFlow where this method is called from.
  setInitialised( true );
}

} // namespace scene_decoder
} // namespace apps
} // namespace visr
