/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "bunch_renderer.hpp"

#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <algorithm>
#include <cmath>

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
  // Helper function to create contiguous ranges.
  ril::AudioChannelIndexVector indexRange( std::size_t startIdx, std::size_t endIdx )
  {
    std::size_t const numElements = endIdx > startIdx ? endIdx - startIdx : 0;
    return ril::AudioChannelIndexVector( ril::AudioChannelSlice( startIdx, numElements, 1 ) );
  }
}

BunchRenderer::BunchRenderer( ril::SignalFlowContext & context,
                                    char const * name,
                                    ril::CompositeComponent * parent,
                                    panning::LoudspeakerArray const & loudspeakerConfiguration,
                                    std::size_t numberOfInputs,
                                    std::size_t numberOfOutputs,
                                    std::size_t interpolationPeriod,
                                    efl::BasicMatrix<ril::SampleType> const & diffusionFilters,
                                    std::string const & trackingConfiguration,
                                    std::size_t sceneReceiverPort,
                                    std::string const & reverbConfig )
 : ril::CompositeComponent( context, name, parent )
 , mSceneReceiver( context, "SceneReceiver", this )
 , mSceneDecoder( context, "SceneDecoder", this )
 , mCoreRenderer( context, "CoreRenderer", this,
     loudspeakerConfiguration, numberOfInputs, numberOfOutputs,
     interpolationPeriod, diffusionFilters, trackingConfiguration )
 , mInput( "input", *this )
 , mOutput( "output", *this )
{
  mInput.setWidth(numberOfInputs);
  mOutput.setWidth(numberOfOutputs);

  mSceneReceiver.setup( sceneReceiverPort, rcl::UdpReceiver::Mode::Synchronous );
  mSceneDecoder.setup( );
   // mCoreRenderer is initialised in the ctor

  registerParameterConnection("SceneReceiver", "messageOutput", "SceneDecoder", "datagramInput");
  registerParameterConnection("SceneDecoder", "objectVectorOutput", "CoreRenderer", "objectDataInput" );

  registerAudioConnection("", "input", indexRange(0, numberOfInputs), "CoreRenderer", "audioIn", indexRange(0, numberOfInputs) );
  registerAudioConnection("CoreRenderer", "audioOut", indexRange(0, numberOfOutputs), "", "output", indexRange(0, numberOfInputs) );
}

BunchRenderer::~BunchRenderer( )
{
}


} // namespace signalflows
} // namespace visr
