/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_network_encoder.hpp"

#include <ciso646>
#include <cstdint>
#include <limits>

namespace visr
{
namespace apps
{
namespace audio_network_streamer
{

  AudioNetworkEncoder::AudioNetworkEncoder( SignalFlowContext const & context,
                                            char const * name,
                                            CompositeComponent * parent )
    : AtomicComponent( context, name, parent )
    , mInput( "in", *this )
{
}

AudioNetworkEncoder::~AudioNetworkEncoder() = default;

void AudioNetworkEncoder::setup(std::size_t width, std::size_t blockLength)
{
  mInput.setWidth( width );
  mBlockLength = blockLength;

// TODO: Allocate parameter output ports;
//     , mMessageOutputs( "messageOut", *this, pml::EmptyParameterConfig() );
}

void AudioNetworkEncoder::process()
{
#if 0
  if( outputMessages.size() != mInput.width() )
  {
    throw std::invalid_argument( "AudioNetworkEncoder::process(): The number of output message queues does not match the number of input audio signals" );
  }
  for( std::size_t idx(0); idx < mInput.width(); ++idx )
  {
    ril::SampleType const * const signal = mInput[idx];
    outputMessages[idx].enqueue( encodeSignal( signal, mBlockLength ) );
  }
#endif
}

std::string AudioNetworkEncoder::encodeSignal( SampleType const * signal, std::size_t length )
{
  static constexpr SampleType scaleFactor = static_cast<SampleType>(std::numeric_limits<std::int16_t>::max());
  std::string ret( 2 * length, '\0' );
  for( std::size_t idx(0); idx < length; ++idx )
  {
    SampleType const saturated = std::min( std::max( signal[idx], static_cast<SampleType>(-1.0) ), static_cast<SampleType>(1.0) );
    SampleType const scaled = saturated * scaleFactor;
    int16_t const intVal = static_cast<std::int16_t>(scaled);
    ret[idx*2] = static_cast<char>(intVal bitand 0xFF);
    ret[idx*2+1] = static_cast<char>( (intVal bitand 0xFF00)>>8 );
  }
  return ret;
}

} // audio_network_encoder
} // namespace apps
} // namespace visr
