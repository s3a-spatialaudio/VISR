/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_PORTAUDIO_INTERFACE_HPP_INCLUDED
#define VISR_LIBRRL_PORTAUDIO_INTERFACE_HPP_INCLUDED

#include <libril/audio_interface.hpp>

#include <libril/communication_area.hpp>
#include <libril/constants.hpp>

#include <memory>
#include <string>
#include <vector>

namespace visr
{
namespace rrl
{

class PortaudioInterface: public ril::AudioInterface
{
public:
  /**
   * Structure to hold all configuration arguments for a PortAudioInterface instance.
   */
  struct Config
  {
  public:
    /** Default contructor to initialise elements to defined values. */
    Config()
      : mNumberOfCaptureChannels( 0 )
      , mNumberOfPlaybackChannels( 0 )
      , mPeriodSize( 0 )
      , mSampleRate( 0 )
      , mSampleFormat( SampleFormat::float32Bit )
      , mInterleaved( false )
      , mHostApi( "" )
    {}

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

    bool mInterleaved;

    /**
     * A string determining the host API to be used for portaudio.
     * At the moment, admissible values are 'default' on all platforms 'DirectSound', 'MME', 'ASIO' 'SoundManager', 'CoreAudio', 'OSS', ALSA', AL',
     * 'WDMKS', 'JACK''WASAPI'
     */
    std::string mHostApi;

  };

  using Base = ril::AudioInterface;

  explicit PortaudioInterface( Config const & config );

  ~PortaudioInterface( );

  /* virtual */ void start();

  /* virtual */ void stop();

  /*virtual*/ bool registerCallback( AudioCallback callback, void* userData );

  /*virtual*/ bool unregisterCallback( AudioCallback audioCallback );
private:
  /**
   * Private implementation class to avoid dependencies to the Portaudio library in the public interface.
   */
  class Impl;

  /**
   * Private implementation object according to the "pointer to implementation" (pimpl) idiom.
   */
  std::unique_ptr<Impl> mImpl;
};

} // namespace rrl
} // namespace visr

#endif // #ifndef VISR_LIBRRL_PORTAUDIO_INTERFACE_HPP_INCLUDED
