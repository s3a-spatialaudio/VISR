/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_PORTAUDIO_INTERFACE_HPP_INCLUDED
#define VISR_LIBRRL_PORTAUDIO_INTERFACE_HPP_INCLUDED

#include <librrl/audio_interface.hpp>
#include <libril/constants.hpp>

#include <memory>
#include <string>
#include <vector>

namespace visr
{
namespace audiointerfaces
{

class PortaudioInterface: public rrl::AudioInterface
{
public:
  /**
   * Structure to hold all configuration arguments for a PortAudioInterface instance.
   */
  struct Config
  {
  public:
      class SampleFormat
      {
      public:
          enum Type
          {
              signedInt8Bit,
              unsignedInt8Bit,
              signedInt16Bit,
              unsignedInt16Bit,
              signedInt24Bit,
              unsignedInt24Bit,
              signedInt32Bit,
              unsignedInt32Bit,
              float32Bit
          };
      };
      SampleFormat::Type mSampleFormat;

//    /** Default contructor to initialise elements to defined values. */
//    Config()
//      : mSampleFormat( SampleFormat::float32Bit )
//      , mInterleaved( false )
//      , mHostApi( "" )
//    {}
// 
      
      Config( SampleFormat::Type sampleFormat, bool interleaved, std::string mHostApi)
      : mSampleFormat(sampleFormat)
      , mInterleaved(interleaved)
      , mHostApi(mHostApi)
      {

      }

    std::size_t mNumberOfCaptureChannels;
    std::size_t mNumberOfPlaybackChannels;
    std::size_t mPeriodSize;

    /**
     * Todo: Consider moving this definition to a more general place.
    */
    using SamplingRateType = std::size_t;
    SamplingRateType mSampleRate;

    /**
     * Enumeration for a type-independent sample format specification
     * TODO: Move to a more general location (for use by all audio interfaces)
     */

    bool mInterleaved;

    /**
     * A string determining the host API to be used for portaudio.
     * At the moment, admissible values are 'default' on all platforms 'DirectSound', 'MME', 'ASIO' 'SoundManager', 'CoreAudio', 'OSS', ALSA', AL',
     * 'WDMKS', 'JACK''WASAPI'
     */
    std::string mHostApi;

  };

  using Base = rrl::AudioInterface;
  
  explicit PortaudioInterface( Configuration const & baseConfig,  std::string const & config);
  
    
  ~PortaudioInterface( );

    
  /* virtual */ void start() override;

  /* virtual */ void stop() override;

  /*virtual*/ bool registerCallback( AudioCallback callback, void* userData ) override;

  /*virtual*/ bool unregisterCallback( AudioCallback audioCallback ) override;
    
    
private:
  /**
   * Private implementation class to avoid dependencies to the Portaudio library in the public interface.
   */
  class Impl;
//  PortaudioInterface::Config parseSpecificConf( std::string const & config );
  /**
   * Private implementation object according to the "pointer to implementation" (pimpl) idiom.
   */
  std::unique_ptr<Impl> mImpl;
};

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_PORTAUDIO_INTERFACE_HPP_INCLUDED
