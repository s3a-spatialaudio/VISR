/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "audio_interface.hpp"


namespace visr
{
namespace rrl
{
  AudioInterface::Configuration::Configuration( std::size_t numCaptureChannels,
                                               std::size_t numPlaybackChannels,
                                               SampleRateType sampleRate,
                                               std::size_t periodSize
                                               ):
  mNumCaptureChannels(numCaptureChannels),
  mNumPlaybackChannels(numPlaybackChannels),
  mSampleRate(sampleRate),
  mPeriodSize(periodSize)
  {
    
  }
  
  AudioInterface::Configuration::~Configuration()= default;
  
} // namespace rrl
} // namespace visr


