/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "baseline_renderer.hpp"

#include <libpanning/XYZ.h>
#include <libpanning/LoudspeakerArray.h>

#include <libpml/biquad_parameter.hpp>

#include <librcl/biquad_iir_filter.hpp>

#include <boost/filesystem.hpp>

#include <algorithm>
#include <cstdio>
#include <sstream>
#include <vector>

namespace visr
{
  namespace signalflows
  {

    namespace
    {
      // create a helper function in an unnamed namespace

      /**
       * Create a vector of unsigned integers ranging from \p start to \p end - 1.
       * @param startIdx the start index of the sequence.
       * @param endIdx The index value one past the end
       * @note Compared to other versions of this function, \p endIdx is the 'past the end' value here, as common in C++ STL conventions.
       * that is indexRange( n, n ) returns an empty vector.
       */
      std::vector<std::size_t> indexRange( std::size_t startIdx, std::size_t endIdx )
      {
        if( endIdx <= startIdx )
        {
          return std::vector<std::size_t>();
        }
        std::size_t const vecLength( endIdx - startIdx  );
        std::vector < std::size_t> ret( vecLength );
        std::generate( ret.begin(), ret.end(), [&] { return startIdx++; } );
        return ret;
      }
    }

    BaselineRenderer::BaselineRenderer( panning::LoudspeakerArray const & loudspeakerConfiguration,
                                       std::size_t numberOfInputs,
                                       std::size_t numberOfOutputs,
                                       std::size_t interpolationPeriod,
                                       efl::BasicMatrix<ril::SampleType> const & diffusionFilters,
                                       std::string const & trackingConfiguration,
                                       std::size_t sceneReceiverPort,
                                       std::size_t numberOfObjectEqSections,
                                       std::size_t period,
                                       ril::SamplingFrequencyType samplingFrequency )
    : AudioSignalFlow( period, samplingFrequency )
    , mDiffusionFilters( diffusionFilters )
    , mSceneReceiver( *this, "SceneReceiver" )
    , mSceneDecoder( *this, "SceneDecoder" )
    , mObjectInputGainEqCalculator( *this, "ObjectGainEqCalculator" )
    , mObjectGain( *this, "ObjectGain" )
    , mObjectEq( *this, "ObjectEq" )
    , mChannelObjectRoutingCalculator( *this, "ChannelObjectRoutingCalculator" )
    , mChannelObjectRouting( *this, "ChannelObjectRouting" )
    , mOutputAdjustment( *this, "OutputAdjustment" )
    , mGainCalculator( *this, "VbapGainCalculator" )
    , mAllradGainCalculator( *this, "AllRadGainGalculator" )
    , mDiffusionGainCalculator( *this, "DiffusionCalculator" )
    , mVbapMatrix( *this, "VbapGainMatrix" )
    , mDiffusePartMatrix( *this, "DiffusePartMatrix" )
    , mDiffusePartDecorrelator( *this, "DiffusePartDecorrelator" )
    , mDirectDiffuseMix( *this, "DirectDiffuseMixer" )
    , mSubwooferMix( *this, "SubwooferMixer" )
    , mNullSource( *this, "NullSource" )
    , mObjectGainParameter( ril::cVectorAlignmentSamples)
    , mObjectEqParameter( 0, 0 )
    , mDiffuseGains( ril::cVectorAlignmentSamples )
    {
      std::size_t const numberOfLoudspeakers = loudspeakerConfiguration.getNumRegularSpeakers();
      std::size_t const numberOfSubwoofers = loudspeakerConfiguration.getNumSubwoofers();
      std::size_t const numberOfOutputSignals = numberOfLoudspeakers + numberOfSubwoofers;

      mTrackingEnabled = not trackingConfiguration.empty( );
      if( mTrackingEnabled )
      {
        // Instantiate the objects.
        mListenerCompensation.reset( new rcl::ListenerCompensation( *this, "TrackingListenerCompensation" ) );
        mSpeakerCompensation.reset( new rcl::DelayVector( *this, "TrackingSpeakerCompensation" ) );
        mTrackingReceiver.reset( new rcl::UdpReceiver( *this, "TrackingReceiver" ) );
        mPositionDecoder.reset( new rcl::PositionDecoder( *this, "TrackingPositionDecoder" ) );

        // for the very moment, do not parse any options, but use hard-coded option values.
        ril::SampleType const cMaxDelay = 1.0f; // maximum delay (in seconds)
        unsigned short cTrackingUdpPort = 8888;
        mListenerCompensation->setup( loudspeakerConfiguration );
        // We start with a initial gain of 0.0 to suppress transients on startup.
        mSpeakerCompensation->setup( numberOfLoudspeakers, period, cMaxDelay,
                                    rcl::DelayVector::InterpolationType::NearestSample,
                                    0.0f, 0.0f );
        mTrackingReceiver->setup( cTrackingUdpPort, rcl::UdpReceiver::Mode::Synchronous );
        mPositionDecoder->setup( panning::XYZ( +2.08f, 0.0f, 0.0f ) );

        mCompensationGains.resize( numberOfLoudspeakers );
        mCompensationDelays.resize( numberOfLoudspeakers );
      }

      bool const outputEqSupport = loudspeakerConfiguration.outputEqualisationPresent();
      if( outputEqSupport )
      {
        std::size_t const outputEqSections = loudspeakerConfiguration.outputEqualisationNumberOfBiquads();
        pml::BiquadParameterMatrix<Afloat> const & eqConfig = loudspeakerConfiguration.outputEqualisationBiquads();
        if( numberOfOutputSignals != eqConfig.numberOfFilters() )
        {
          throw std::invalid_argument( "BaselineRenderer: Size of the output EQ configuration config differs from "
                                      "the number of output signals (regular loudspeakers + subwoofers).");
        }
        mOutputEqualisationFilter.reset( new rcl::BiquadIirFilter( *this, "OutputEqualisationFilter" ) );
        mOutputEqualisationFilter->setup( numberOfOutputSignals, outputEqSections, eqConfig );
      }

      mSceneReceiver.setup( sceneReceiverPort, rcl::UdpReceiver::Mode::Synchronous );
      mSceneDecoder.setup( );
      mObjectInputGainEqCalculator.setup( numberOfInputs, numberOfObjectEqSections );
      mObjectGain.setup( numberOfInputs, interpolationPeriod, 0.0 /*No delay*/,
                        rcl::DelayVector::InterpolationType::NearestSample );
      mObjectEq.setup( numberOfInputs, numberOfObjectEqSections );

      mChannelObjectRoutingCalculator.setup( numberOfInputs, loudspeakerConfiguration);
      mChannelObjectRouting.setup(numberOfInputs, numberOfLoudspeakers);
      mGainCalculator.setup( numberOfInputs, loudspeakerConfiguration );
      mVbapMatrix.setup( numberOfInputs, numberOfLoudspeakers, interpolationPeriod, 0.0f );

      //////////////////////////////////////////////////////////////////////////////////////
      // HOA decoding support
      boost::filesystem::path const regArrayPath( CMAKE_SOURCE_DIR "/src/libpanning/test/matlab/arrays/t-design_t8_P40.xml" );
      boost::filesystem::path const regArrayDecodeMtxPath( CMAKE_SOURCE_DIR "/src/libpanning/test/matlab/arrays/decode_N8_P40_t-design_t8_P40.txt" );
      if( not exists( regArrayPath ) or is_directory( regArrayPath ) )
      {
        throw std::invalid_argument( "Path to regular array does not exist." );
      }
      if( not exists( regArrayDecodeMtxPath ) or is_directory( regArrayDecodeMtxPath ) )
      {
        throw std::invalid_argument( "Path to HOA decoding matrix does not exist." );
      }
      pml::MatrixParameter<Afloat> allRadDecoderGains = pml::MatrixParameter<Afloat>::fromTextFile( regArrayDecodeMtxPath.string() );

      panning::LoudspeakerArray allRadRegArray;
      allRadRegArray.loadXmlFile( regArrayPath.string() );
      mAllradGainCalculator.setup( allRadRegArray, loudspeakerConfiguration, allRadDecoderGains );

      //////////////////////////////////////////////////////////////////////////////////////

      mDiffusionGainCalculator.setup( numberOfInputs );
      mDiffusePartMatrix.setup( numberOfInputs, 1, interpolationPeriod, 0.0f );

      /**
       * Adjust the level of the diffuse objects such that they are comparable to point sources.
       * Here we assume that the decorrelated signals are ideally decorrelated. Note that this is not
       * the case with the current set of decorrelation filters.
       * @todo Also consider a more elaborate panning law between the direct and diffuse part of a single source.
       */
      ril::SampleType const diffusorGain = static_cast<ril::SampleType>(1.0) / std::sqrt( static_cast<ril::SampleType>(numberOfLoudspeakers) );
      mDiffusePartDecorrelator.setup( numberOfLoudspeakers, mDiffusionFilters, diffusorGain );
      mDirectDiffuseMix.setup( numberOfLoudspeakers, 3 ); // Third input is for the channel routing.
      mNullSource.setup( 1/*width*/ );

      efl::BasicVector<ril::SampleType> const & outputGains =loudspeakerConfiguration.getGainAdjustment();
      efl::BasicVector<ril::SampleType> const & outputDelays = loudspeakerConfiguration.getDelayAdjustment();

      Afloat const * const maxEl = std::max_element( outputDelays.data(),
                                                    outputDelays.data()+outputDelays.size() );
      Afloat const maxDelay = std::ceil( *maxEl ); // Sufficient for nearestSample even if there is no particular compensation for the interpolation method's delay inside.

      mOutputAdjustment.setup( numberOfOutputSignals, period, maxDelay, rcl::DelayVector::InterpolationType::NearestSample,
                              outputDelays, outputGains );

      // Note: This assumes that the type 'Afloat' used in libpanning is
      // identical to ril::SampleType (at the moment, both are floats).
      efl::BasicMatrix<ril::SampleType> const & subwooferMixGains = loudspeakerConfiguration.getSubwooferGains();
      mSubwooferMix.setup( numberOfLoudspeakers, numberOfSubwoofers, 0/*interpolation steps*/, subwooferMixGains );

      // Create the index vectors for connecting the ports.
      // First, create the start indices for all output vectors by adding the width of the previous vector.
      std::size_t const captureStartIdx = 0;
      std::size_t const objectGainOutStartIdx = captureStartIdx + numberOfInputs;
      std::size_t const objectEqOutStartIdx = objectGainOutStartIdx + numberOfInputs;
      std::size_t const channelObjectRoutingOutStartIdx = objectEqOutStartIdx + numberOfInputs;
      std::size_t const vbapMatrixOutStartIdx = channelObjectRoutingOutStartIdx + numberOfLoudspeakers;
      std::size_t const diffuseMixerOutStartIdx = vbapMatrixOutStartIdx + numberOfLoudspeakers;
      std::size_t const decorrelatorOutStartIdx = diffuseMixerOutStartIdx + 1;
      std::size_t const mixOutStartIdx = decorrelatorOutStartIdx + numberOfLoudspeakers;
      std::size_t const trackingCompensationOutStartIdx = mixOutStartIdx + numberOfLoudspeakers;
      std::size_t const subwooferMixerOutStartIdx = trackingCompensationOutStartIdx + (mTrackingEnabled ? numberOfLoudspeakers : 0 );
      std::size_t const outputEqualisationOutStartIdx = subwooferMixerOutStartIdx + numberOfSubwoofers;
      std::size_t const outputAdjustOutStartIdx = outputEqualisationOutStartIdx + (outputEqSupport ? numberOfOutputSignals : 0);
      std::size_t const nullSourceOutStartIdx = outputAdjustOutStartIdx + numberOfOutputSignals;
      std::size_t const communicationChannelEndIndex = nullSourceOutStartIdx + 1; // One past end index. Also the number of total indices.

      // Now, explicitly construct the index vectors.
      std::vector<ril::AudioPort::SignalIndexType> captureChannels = indexRange( captureStartIdx, objectGainOutStartIdx );
      std::vector<ril::AudioPort::SignalIndexType> const objectGainOutChannels = indexRange( objectGainOutStartIdx, objectEqOutStartIdx );
      std::vector<ril::AudioPort::SignalIndexType> const objectEqOutChannels = indexRange( objectEqOutStartIdx, channelObjectRoutingOutStartIdx );
      std::vector<ril::AudioPort::SignalIndexType> const channelObjectRoutingOutChannels = indexRange( channelObjectRoutingOutStartIdx, vbapMatrixOutStartIdx );
      std::vector<ril::AudioPort::SignalIndexType> const vbapMatrixOutChannels = indexRange( vbapMatrixOutStartIdx, diffuseMixerOutStartIdx );
      std::vector<ril::AudioPort::SignalIndexType> const diffuseMixerOutChannels = indexRange( diffuseMixerOutStartIdx, decorrelatorOutStartIdx );
      std::vector<ril::AudioPort::SignalIndexType> const decorrelatorOutChannels = indexRange( decorrelatorOutStartIdx, mixOutStartIdx );
      std::vector<ril::AudioPort::SignalIndexType> const mixOutChannels = indexRange( mixOutStartIdx, trackingCompensationOutStartIdx );
      std::vector<ril::AudioPort::SignalIndexType> const trackingCompensationOutChannels = indexRange( trackingCompensationOutStartIdx, subwooferMixerOutStartIdx );
      std::vector<ril::AudioPort::SignalIndexType> const subwooferMixerOutChannels = indexRange( subwooferMixerOutStartIdx, outputEqualisationOutStartIdx );
      std::vector<ril::AudioPort::SignalIndexType> const outputEqualisationOutChannels = indexRange( outputEqualisationOutStartIdx, outputAdjustOutStartIdx );
      std::vector<ril::AudioPort::SignalIndexType> const outputAdjustOutChannels = indexRange( outputAdjustOutStartIdx, nullSourceOutStartIdx );
      std::vector<ril::AudioPort::SignalIndexType> const nullSourceOutChannels = indexRange( nullSourceOutStartIdx, communicationChannelEndIndex );

      // The playback indices "do" all the signal routing to the final output channels.
      // THe initial value means the all channels that are not assigned get their signal from the NullSource component.
      std::vector<ril::AudioPort::SignalIndexType> playbackChannels( numberOfOutputs, nullSourceOutStartIdx );
      for( std::size_t lspIdx(0); lspIdx < numberOfLoudspeakers; ++lspIdx )
      {
        panning::LoudspeakerArray::ChannelIndex const outIdx = loudspeakerConfiguration.channelIndex( lspIdx );
        if( outIdx < 0 or outIdx >= static_cast<panning::LoudspeakerArray::ChannelIndex>(numberOfOutputs) )
        {
          std::stringstream msg;
          msg << "Invalid channel index " << outIdx
          << " for loudspeaker " << lspIdx << ", maximum admissible index: " << numberOfOutputs << ".";
          throw std::invalid_argument( msg.str() );
        }
        playbackChannels.at( outIdx ) = outputAdjustOutStartIdx + lspIdx;
      }
      for( std::size_t subIdx(0); subIdx < numberOfSubwoofers; ++subIdx )
      {
        panning::LoudspeakerArray::ChannelIndex const outIdx = loudspeakerConfiguration.getSubwooferChannel( subIdx );
        if( outIdx < 0 or outIdx >= static_cast<panning::LoudspeakerArray::ChannelIndex>(numberOfOutputs) )
        {
          std::stringstream msg;
          msg << "Invalid channel index " << outIdx
          << " for subwoofer " << subIdx << ", maximum admissible index: " << numberOfOutputs << ".";
          throw std::invalid_argument( msg.str() );
        }
        // The subwoofer channels are located behind the regular loudspeakers.
        playbackChannels.at( outIdx ) = outputAdjustOutStartIdx + numberOfLoudspeakers + subIdx;
      }

      initCommArea( communicationChannelEndIndex, period, ril::cVectorAlignmentSamples );

      // Connect the ports
      assignCommunicationIndices( "ObjectGain", "in", captureChannels );
      assignCommunicationIndices( "ObjectGain", "out", objectGainOutChannels );
      assignCommunicationIndices( "ObjectEq", "in", objectGainOutChannels );
      assignCommunicationIndices( "ObjectEq", "out", objectEqOutChannels );
      assignCommunicationIndices( "ChannelObjectRouting", "in", objectEqOutChannels);
      assignCommunicationIndices( "VbapGainMatrix", "in", objectEqOutChannels );
      assignCommunicationIndices( "DiffusePartMatrix", "in", objectEqOutChannels );
      assignCommunicationIndices( "ChannelObjectRouting", "out", channelObjectRoutingOutChannels );
      assignCommunicationIndices( "VbapGainMatrix", "out", vbapMatrixOutChannels );
      assignCommunicationIndices( "DiffusePartMatrix", "out", diffuseMixerOutChannels );
      assignCommunicationIndices( "DiffusePartDecorrelator", "in", diffuseMixerOutChannels );
      assignCommunicationIndices( "DiffusePartDecorrelator", "out", decorrelatorOutChannels );
      assignCommunicationIndices( "DirectDiffuseMixer", "in0", vbapMatrixOutChannels );
      assignCommunicationIndices( "DirectDiffuseMixer", "in1", decorrelatorOutChannels );
      assignCommunicationIndices( "DirectDiffuseMixer", "in2", channelObjectRoutingOutChannels );
      assignCommunicationIndices( "DirectDiffuseMixer", "out", mixOutChannels );

      std::vector<ril::AudioPort::SignalIndexType> outputAdjustInChannels( numberOfOutputSignals );
      if( mTrackingEnabled )
      {
        assignCommunicationIndices( "TrackingSpeakerCompensation", "in", mixOutChannels );
        assignCommunicationIndices( "TrackingSpeakerCompensation", "out", trackingCompensationOutChannels );
        assignCommunicationIndices( "SubwooferMixer", "in", trackingCompensationOutChannels );

        std::copy( trackingCompensationOutChannels.begin(), trackingCompensationOutChannels.end(), outputAdjustInChannels.begin()  );
      }
      else
      {
        assignCommunicationIndices( "SubwooferMixer", "in", mixOutChannels );
        std::copy( mixOutChannels.begin(), mixOutChannels.end(), outputAdjustInChannels.begin()  );
      }
      assignCommunicationIndices( "SubwooferMixer", "out", subwooferMixerOutChannels );
      std::copy( subwooferMixerOutChannels.begin(), subwooferMixerOutChannels.end(), outputAdjustInChannels.begin() + numberOfLoudspeakers );

      assignCommunicationIndices( "NullSource", "out", nullSourceOutChannels );

      if( outputEqSupport )
      {
        assignCommunicationIndices( "OutputEqualisationFilter", "in", outputAdjustInChannels );
        assignCommunicationIndices( "OutputEqualisationFilter", "out", outputEqualisationOutChannels );
        assignCommunicationIndices( "OutputAdjustment", "in", outputEqualisationOutChannels );
      }
      else
      {
        assignCommunicationIndices( "OutputAdjustment", "in", outputAdjustInChannels );
      }
      assignCommunicationIndices( "OutputAdjustment", "out", outputAdjustOutChannels );

      assignCaptureIndices( &captureChannels[0], captureChannels.size( ) );
      assignPlaybackIndices( &playbackChannels[0], playbackChannels.size( ) );

      mObjectGainParameter.resize( numberOfInputs );

      mObjectEqParameter.resize( numberOfInputs, numberOfObjectEqSections );

      mGainParameters.resize( numberOfLoudspeakers, numberOfInputs );

      mDiffuseGains.resize( 1, numberOfInputs );

      // should not be done here, but in AudioSignalFlow where this method is called from.
      setInitialised( true );
    }

    BaselineRenderer::~BaselineRenderer( )
    {
    }

    /*virtual*/ void
    BaselineRenderer::process()
    {
      mSceneReceiver.process( mSceneMessages );
      mSceneDecoder.process( mSceneMessages, mObjectVector );
      if( mTrackingEnabled )
      {
        mTrackingReceiver->process( mTrackingMessages );
        mPositionDecoder->process( mTrackingMessages, mListenerPosition );
        mGainCalculator.setListenerPosition( mListenerPosition );
        mListenerCompensation->process( mListenerPosition, mCompensationGains, mCompensationDelays );
      }
      mObjectInputGainEqCalculator.process( mObjectVector, mObjectGainParameter, mObjectEqParameter );
      mObjectGain.setGain( mObjectGainParameter );
      mObjectGain.process();
      mObjectEq.setCoefficientMatrix( mObjectEqParameter );
      mObjectEq.process();
      mChannelObjectRoutingCalculator.process(mObjectVector, mChannelObjectRoutings);
      mChannelObjectRouting.setRouting(mChannelObjectRoutings);
      mChannelObjectRouting.process();
      mGainCalculator.process( mObjectVector, mGainParameters );
      // Calculate the panning gains for HOA objects and place them into the panning gain matrix.
      mAllradGainCalculator.process( mObjectVector, mGainParameters );
      mDiffusionGainCalculator.process( mObjectVector, mDiffuseGains );
      mVbapMatrix.setGains( mGainParameters );
      mVbapMatrix.process();
      mDiffusePartMatrix.setGains( mDiffuseGains );
      mDiffusePartMatrix.process();
      mDiffusePartDecorrelator.process();
      mDirectDiffuseMix.process();
      mSubwooferMix.process();
      if( mTrackingEnabled )
      {
        mSpeakerCompensation->setDelayAndGain( mCompensationDelays, mCompensationGains );
        mSpeakerCompensation->process( );
      }
      if( mOutputEqualisationFilter )
      {
        mOutputEqualisationFilter->process();
      }
      mOutputAdjustment.process();
      mNullSource.process();
    }
    
  } // namespace signalflows
} // namespace visr
