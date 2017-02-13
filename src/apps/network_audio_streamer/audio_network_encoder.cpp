/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_network_encoder.hpp"

#include <libpml/message_queue.hpp>

#include <ciso646>
#include <cstdint>
#include <limits>
//#include <memory>
//#include <sstream>

namespace visr
{
namespace apps
{
namespace audio_network_streamer
{

AudioNetworkEncoder::AudioNetworkEncoder( ril::AudioSignalFlow& container, char const * name )
 : AudioComponent( container, name )
 , mInput( "in", *this )
{
}

AudioNetworkEncoder::~AudioNetworkEncoder()
{
}

void AudioNetworkEncoder::setup(std::size_t width, std::size_t blockLength)
{
  mInput.setWidth( width );
  mBlockLength = blockLength;
}

void AudioNetworkEncoder::process(std::vector<pml::MessageQueue<std::string> > & outputMessages)
{
  if( outputMessages.size() != mInput.width() )
  {
    throw std::invalid_argument( "AudioNetworkEncoder::process(): The number of output message queues does not match the number of input audio signals" );
  }
  for( std::size_t idx(0); idx < mInput.width(); ++idx )
  {
    ril::SampleType const * const signal = mInput[idx];
    outputMessages[idx].enqueue( encodeSignal( signal, mBlockLength ) );
  }
}

std::string AudioNetworkEncoder::encodeSignal( ril::SampleType const * signal, std::size_t length )
{
  static constexpr ril::SampleType scaleFactor = static_cast<ril::SampleType>(std::numeric_limits<std::int16_t>::max());
  std::string ret( 2 * length, '\0' );
  for( std::size_t idx(0); idx < length; ++idx )
  {
    ril::SampleType const saturated = std::min( std::max( signal[idx], static_cast<ril::SampleType>(-1.0) ), static_cast<ril::SampleType>(1.0) );
    ril::SampleType const scaled = saturated * scaleFactor;
    std::int16_t const intVal = static_cast<std::int16_t>(scaled);
    ret[idx*2] = static_cast<char>(intVal bitand 0xFF);
    ret[idx*2+1] = static_cast<char>( (intVal bitand 0xFF00)>>8 );
  }
  return ret;
}

} // audio_network_encoder
} // namespace apps
} // namespace visr
