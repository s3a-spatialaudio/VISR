/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRRL_PORTAUDIO_INTERFACE_HPP_INCLUDED
#define VISR_LIBRRL_PORTAUDIO_INTERFACE_HPP_INCLUDED

#include "audio_interface.hpp"
#include "export_symbols.hpp"

#include <libvisr/constants.hpp>

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace visr
{
namespace audiointerfaces
{

class VISR_AUDIOINTERFACES_LIBRARY_SYMBOL 
PortaudioInterface: public audiointerfaces::AudioInterface
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
      
      using TranslateSampleFormatMapTypeString = std::map<std::string, SampleFormat::Type >;
      TranslateSampleFormatMapTypeString const cTranslateSampleFormatMapString = {
          {"signedInt8Bit", SampleFormat::signedInt8Bit },
          {"unsignedInt8Bit", SampleFormat::unsignedInt8Bit },
          {"signedInt16Bit", SampleFormat::signedInt16Bit },
          {"unsignedInt16Bit", SampleFormat::unsignedInt16Bit },
          {"signedInt24Bit", SampleFormat::signedInt24Bit },
          {"unsignedInt24Bit", SampleFormat::unsignedInt24Bit },
          {"signedInt32Bit", SampleFormat::signedInt32Bit },
          {"unsignedInt32Bit", SampleFormat::unsignedInt32Bit },
          {"float32Bit", SampleFormat::float32Bit }
      };

      SampleFormat::Type translateToSampleFormat( std::string format)
      {
          auto const findIt = cTranslateSampleFormatMapString.find( format );
          if( findIt == cTranslateSampleFormatMapString.end() ) {
              throw std::invalid_argument( "The given sample format does not match a sample format in portaudio." );
          }
          return findIt->second;
      }
      
    Config( std::string sampleFormat, bool interleaved, std::string mHostApi);

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

    /**
     * Name of the PortAudio input device.
     * If left empty, the default input for the given host API will be used.
     */
    std::string mInputDeviceName;

    /**
     * Name of the PortAudio output device.
     * If left empty, the default output for the given host API will be used.
     */
    std::string mOutputDeviceName;
  };

  using Base = audiointerfaces::AudioInterface;
  
  explicit PortaudioInterface( Configuration const & baseConfig,  std::string const & config);

  ~PortaudioInterface( );

    
  /* virtual */ void start() override;

  /* virtual */ void stop() override;

  /*virtual*/ bool registerCallback( AudioCallback callback, void* userData ) override;

  /*virtual*/ bool unregisterCallback( AudioCallback audioCallback ) override;
    
  /**
   * Return the number of input channels to the interface.
   */
  /*virtual*/ std::size_t numberOfCaptureChannels() const override;

  /**
   * Return the number of output channels to the interface.
   */
  /*virtual*/ std::size_t numberOfPlaybackChannels() const override;

  /**
   * Return the configured period (block size).
   */
  /*virtual*/ std::size_t period() const override;

  /**
   * Return the configured sampling frequency (in Hz)
   */
  /*virtual*/ std::size_t samplingFrequency() const override;

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
