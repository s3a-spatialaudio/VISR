/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "baseline_renderer.hpp"

#include <libpanning/LoudspeakerArray.h>

#include <libpml/biquad_parameter.hpp>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <vector>

namespace visr
{
namespace signalflows
{

BaselineRenderer::BaselineRenderer( SignalFlowContext const & context,
  char const * name,
  CompositeComponent * parent,
  panning::LoudspeakerArray const & loudspeakerConfiguration,
  std::size_t numberOfInputs,
  std::size_t numberOfOutputs
 )
  : BaselineRenderer( context, name, parent, loudspeakerConfiguration, numberOfInputs, numberOfOutputs, 
                      4096ul,
                      efl::BasicMatrix<SampleType>(numberOfOutputs, 0, cVectorAlignmentSamples ), 
    std::string(), 4242, 0 /*number of EQ sections*/, std::string(), false )
{}

BaselineRenderer::BaselineRenderer( SignalFlowContext const & context,
                                    char const * name,
                                    CompositeComponent * parent,
                                    panning::LoudspeakerArray const & loudspeakerConfiguration,
                                    std::size_t numberOfInputs,
                                    std::size_t numberOfOutputs,
                                    std::size_t interpolationPeriod,
                                    efl::BasicMatrix<SampleType> const & diffusionFilters,
                                    std::string const & trackingConfiguration,
                                    std::size_t sceneReceiverPort,
                                    std::size_t numberOfObjectEqSections,
                                    std::string const & reverbConfig,
                                    bool frequencyDependentPanning )
 : CompositeComponent( context, name, parent )
 , mSceneReceiver( context, "SceneReceiver", this )
 , mSceneDecoder( context, "SceneDecoder", this )
 , mCoreRenderer( context, "CoreRenderer", this, loudspeakerConfiguration, numberOfInputs, numberOfOutputs,
                  interpolationPeriod, diffusionFilters, trackingConfiguration, numberOfObjectEqSections,
                  reverbConfig, frequencyDependentPanning )
 , mInput( "input", *this, numberOfInputs )
 , mOutput( "output", *this, numberOfOutputs )

{
  mSceneReceiver.setup( sceneReceiverPort, rcl::UdpReceiver::Mode::Asynchronous );

  audioConnection( mInput, mCoreRenderer.audioPort( "audioIn") );
  audioConnection( mCoreRenderer.audioPort( "audioOut"), mOutput );
  parameterConnection( mSceneReceiver.parameterPort("messageOutput"), mSceneDecoder.parameterPort("datagramInput") );
  parameterConnection( mSceneDecoder.parameterPort("objectVectorOutput"), mCoreRenderer.parameterPort("objectDataInput") );

  if( not trackingConfiguration.empty() )
  {
    mTrackingReceiver.reset( new rcl::UdpReceiver( context, "TrackingReceiver" ) );
    mTrackingPositionDecoder.reset( new rcl::PositionDecoder( context, "TrackingPositionDecoder" ) );

    mTrackingReceiver->setup( 8888, rcl::UdpReceiver::Mode::Synchronous );
    mTrackingPositionDecoder->setup( panning::XYZ( 0.0f, 0.0f, 0.0f ) );
    parameterConnection( mTrackingPositionDecoder->parameterPort("positionOutput"), mCoreRenderer.parameterPort("trackingPositionInput") );
  }
}

BaselineRenderer::~BaselineRenderer( )
{
}

} // namespace signalflows
} // namespace visr
