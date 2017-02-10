/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_network_encoder.hpp"

#include <libpml/message_queue.hpp>

#include <memory>
#include <sstream>

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
  std::stringstream stream;

  // TODO: Fill in here!
  // Needs information about
  // - Endianness of data stream
  // - unsigned/signed integers.
  // - Consider a conversion function float->int types in libefl
  // TODO: Do not rely on boost::endian which is implemented only froom Boost 1.58 onwards.

  //for( std::size_t sampleIdx(0); sampleIdx < length; ++sampleIdx )
  //{

  //}

  // TODO: implement me.
  return stream.str();
}

} // audio_network_encoder
} // namespace apps
} // namespace visr
