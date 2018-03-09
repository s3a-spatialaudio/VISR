/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "visr_renderer.hpp"

#include <librcl/scene_decoder.hpp>

#include <libpythonsupport/python_wrapper.hpp>

namespace visr
{
namespace signalflows
{

VisrRenderer::VisrRenderer( SignalFlowContext const & context,
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
			    bool frequencyDependentPanning,
			    std::string const & metadapterConfig
			  )
 : CompositeComponent( context, name, parent )
 , mSceneReceiver( context, "SceneReceiver", this )
 , mSceneDecoder( nullptr )
 , mCoreRenderer( context, "CoreRenderer", this, loudspeakerConfiguration, numberOfInputs, numberOfOutputs,
                  interpolationPeriod, diffusionFilters, trackingConfiguration, numberOfObjectEqSections,
                  reverbConfig, frequencyDependentPanning )
 , mInput( "input", *this, numberOfInputs )
 , mOutput( "output", *this, numberOfOutputs )

{
  mSceneReceiver.setup( sceneReceiverPort, rcl::UdpReceiver::Mode::Asynchronous );
  if( metadapterConfig.empty() )
  {
    // std::make_unique (C++14) would be handy.
    std::unique_ptr<rcl::SceneDecoder> sceneDec( new rcl::SceneDecoder( context, "SceneDeoder", this ) );
    sceneDec->setup(); // TODO: Remove setup method from SceneDecoder
    mSceneDecoder =  std::move(sceneDec);
  }
  else
  {
#if VISR_PYTHON_SUPPORT
//    mSceneDecoder.reset( new pythonsupport::PythonWrapper( context, "Metadapter", ))

    
    
#else
    throw std::invalid_argument( "Providing a metadapter configuration requires a VISR built with Python support." );
#endif
  }


  audioConnection( mInput, mCoreRenderer.audioPort( "audioIn") );
  audioConnection( mCoreRenderer.audioPort( "audioOut"), mOutput );
  parameterConnection( mSceneReceiver.parameterPort("messageOutput"), mSceneDecoder->parameterPort("datagramInput") );
  parameterConnection( mSceneDecoder->parameterPort("objectVectorOutput"), mCoreRenderer.parameterPort("objectDataInput") );

  if( not trackingConfiguration.empty() )
  {
    mTrackingReceiver.reset( new rcl::UdpReceiver( context, "TrackingReceiver" ) );
    mTrackingPositionDecoder.reset( new rcl::PositionDecoder( context, "TrackingPositionDecoder" ) );

    mTrackingReceiver->setup( 8888, rcl::UdpReceiver::Mode::Synchronous );
    mTrackingPositionDecoder->setup( panning::XYZ( 0.0f, 0.0f, 0.0f ) );
    parameterConnection( mTrackingPositionDecoder->parameterPort("positionOutput"), mCoreRenderer.parameterPort("trackingPositionInput") );
  }
}

VisrRenderer::~VisrRenderer( )
{
}

} // namespace signalflows
} // namespace visr
